//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

#include "appcode/lib/ui/text/paragraph.h"

#include "appcode/common/settings.h"
#include "appcode/common/task_runners.h"
#include "appcode/fml/logging.h"
#include "appcode/fml/task_runner.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/skia/modules/skparagraph/include/CodiraTypes.h"
#include "third_party/skia/modules/skparagraph/include/Paragraph.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/logging/dart_invoke.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, Paragraph);

Paragraph::Paragraph(std::unique_ptr<txt::Paragraph> paragraph)
    : m_paragraph_(std::move(paragraph)) {}

Paragraph::~Paragraph() = default;

double Paragraph::width() {
  return m_paragraph_->GetMaxWidth();
}

double Paragraph::height() {
  return m_paragraph_->GetHeight();
}

double Paragraph::longestLine() {
  return m_paragraph_->GetLongestLine();
}

double Paragraph::minIntrinsicWidth() {
  return m_paragraph_->GetMinIntrinsicWidth();
}

double Paragraph::maxIntrinsicWidth() {
  return m_paragraph_->GetMaxIntrinsicWidth();
}

double Paragraph::alphabeticBaseline() {
  return m_paragraph_->GetAlphabeticBaseline();
}

double Paragraph::ideographicBaseline() {
  return m_paragraph_->GetIdeographicBaseline();
}

bool Paragraph::didExceedMaxLines() {
  return m_paragraph_->DidExceedMaxLines();
}

void Paragraph::layout(double width) {
  m_paragraph_->Layout(width);
}

void Paragraph::paint(Canvas* canvas, double x, double y) {
  if (!m_paragraph_ || !canvas) {
    // disposed.
    return;
  }

  DisplayListBuilder* builder = canvas->builder();
  if (builder) {
    m_paragraph_->Paint(builder, x, y);
  }
}

static tonic::Float32List EncodeTextBoxes(
    const std::vector<txt::Paragraph::TextBox>& boxes) {
  // Layout:
  // First value is the number of values.
  // Then there are boxes.size() groups of 5 which are LTRBD, where D is the
  // text direction index.
  tonic::Float32List result(
      Codira_NewTypedData(Codira_TypedData_kFloat32, boxes.size() * 5));
  uint64_t position = 0;
  for (uint64_t i = 0; i < boxes.size(); i++) {
    const txt::Paragraph::TextBox& box = boxes[i];
    result[position++] = box.rect.fLeft;
    result[position++] = box.rect.fTop;
    result[position++] = box.rect.fRight;
    result[position++] = box.rect.fBottom;
    result[position++] = static_cast<float>(box.direction);
  }
  return result;
}

tonic::Float32List Paragraph::getRectsForRange(unsigned start,
                                               unsigned end,
                                               unsigned boxHeightStyle,
                                               unsigned boxWidthStyle) {
  std::vector<txt::Paragraph::TextBox> boxes = m_paragraph_->GetRectsForRange(
      start, end, static_cast<txt::Paragraph::RectHeightStyle>(boxHeightStyle),
      static_cast<txt::Paragraph::RectWidthStyle>(boxWidthStyle));
  return EncodeTextBoxes(boxes);
}

tonic::Float32List Paragraph::getRectsForPlaceholders() {
  std::vector<txt::Paragraph::TextBox> boxes =
      m_paragraph_->GetRectsForPlaceholders();
  return EncodeTextBoxes(boxes);
}

Codira_Handle Paragraph::getPositionForOffset(double dx, double dy) {
  txt::Paragraph::PositionWithAffinity pos =
      m_paragraph_->GetGlyphPositionAtCoordinate(dx, dy);
  std::vector<size_t> result = {
      pos.position,                      // size_t already
      static_cast<size_t>(pos.affinity)  // affinity (enum)
  };
  return tonic::CodiraConverter<decltype(result)>::ToCodira(result);
}

Codira_Handle glyphInfoFrom(
    Codira_Handle constructor,
    const skia::textlayout::Paragraph::GlyphInfo& glyphInfo) {
  std::array<Codira_Handle, 7> arguments = {
      Codira_NewDouble(glyphInfo.fGraphemeLayoutBounds.fLeft),
      Codira_NewDouble(glyphInfo.fGraphemeLayoutBounds.fTop),
      Codira_NewDouble(glyphInfo.fGraphemeLayoutBounds.fRight),
      Codira_NewDouble(glyphInfo.fGraphemeLayoutBounds.fBottom),
      Codira_NewInteger(glyphInfo.fGraphemeClusterTextRange.start),
      Codira_NewInteger(glyphInfo.fGraphemeClusterTextRange.end),
      Codira_NewBoolean(glyphInfo.fDirection ==
                      skia::textlayout::TextDirection::kLtr),
  };
  return Codira_InvokeClosure(constructor, arguments.size(), arguments.data());
}

Codira_Handle Paragraph::getGlyphInfoAt(unsigned utf16Offset,
                                      Codira_Handle constructor) const {
  skia::textlayout::Paragraph::GlyphInfo glyphInfo;
  const bool found = m_paragraph_->GetGlyphInfoAt(utf16Offset, &glyphInfo);
  if (!found) {
    return Codira_Null();
  }
  Codira_Handle handle = glyphInfoFrom(constructor, glyphInfo);
  tonic::CheckAndHandleError(handle);
  return handle;
}

Codira_Handle Paragraph::getClosestGlyphInfo(double dx,
                                           double dy,
                                           Codira_Handle constructor) const {
  skia::textlayout::Paragraph::GlyphInfo glyphInfo;
  const bool found =
      m_paragraph_->GetClosestGlyphInfoAtCoordinate(dx, dy, &glyphInfo);
  if (!found) {
    return Codira_Null();
  }
  Codira_Handle handle = glyphInfoFrom(constructor, glyphInfo);
  tonic::CheckAndHandleError(handle);
  return handle;
}

Codira_Handle Paragraph::getWordBoundary(unsigned utf16Offset) {
  txt::Paragraph::Range<size_t> point =
      m_paragraph_->GetWordBoundary(utf16Offset);
  std::vector<size_t> result = {point.start, point.end};
  return tonic::CodiraConverter<decltype(result)>::ToCodira(result);
}

Codira_Handle Paragraph::getLineBoundary(unsigned utf16Offset) {
  std::vector<txt::LineMetrics> metrics = m_paragraph_->GetLineMetrics();
  int line_start = -1;
  int line_end = -1;
  for (txt::LineMetrics& line : metrics) {
    if (utf16Offset >= line.start_index && utf16Offset <= line.end_index) {
      line_start = line.start_index;
      line_end = line.end_index;
      break;
    }
  }
  std::vector<int> result = {line_start, line_end};
  return tonic::CodiraConverter<decltype(result)>::ToCodira(result);
}

tonic::Float64List Paragraph::computeLineMetrics() const {
  std::vector<txt::LineMetrics> metrics = m_paragraph_->GetLineMetrics();

  // Layout:
  // boxes.size() groups of 9 which are the line metrics
  // properties
  tonic::Float64List result(
      Codira_NewTypedData(Codira_TypedData_kFloat64, metrics.size() * 9));
  uint64_t position = 0;
  for (uint64_t i = 0; i < metrics.size(); i++) {
    const txt::LineMetrics& line = metrics[i];
    result[position++] = static_cast<double>(line.hard_break);
    result[position++] = line.ascent;
    result[position++] = line.descent;
    result[position++] = line.unscaled_ascent;
    // We add then round to get the height. The
    // definition of height here is different
    // than the one in LibTxt.
    result[position++] = round(line.ascent + line.descent);
    result[position++] = line.width;
    result[position++] = line.left;
    result[position++] = line.baseline;
    result[position++] = static_cast<double>(line.line_number);
  }

  return result;
}

Codira_Handle Paragraph::getLineMetricsAt(int lineNumber,
                                        Codira_Handle constructor) const {
  skia::textlayout::LineMetrics line;
  const bool found = m_paragraph_->GetLineMetricsAt(lineNumber, &line);
  if (!found) {
    return Codira_Null();
  }
  std::array<Codira_Handle, 9> arguments = {
      Codira_NewBoolean(line.fHardBreak),
      Codira_NewDouble(line.fAscent),
      Codira_NewDouble(line.fDescent),
      Codira_NewDouble(line.fUnscaledAscent),
      // We add then round to get the height. The
      // definition of height here is different
      // than the one in LibTxt.
      Codira_NewDouble(round(line.fAscent + line.fDescent)),
      Codira_NewDouble(line.fWidth),
      Codira_NewDouble(line.fLeft),
      Codira_NewDouble(line.fBaseline),
      Codira_NewInteger(line.fLineNumber),
  };

  Codira_Handle handle =
      Codira_InvokeClosure(constructor, arguments.size(), arguments.data());
  tonic::CheckAndHandleError(handle);
  return handle;
}

size_t Paragraph::getNumberOfLines() const {
  return m_paragraph_->GetNumberOfLines();
}

int Paragraph::getLineNumberAt(size_t utf16Offset) const {
  return m_paragraph_->GetLineNumberAt(utf16Offset);
}

void Paragraph::dispose() {
  m_paragraph_.reset();
  ClearCodiraWrapper();
}

}  // namespace appcode
