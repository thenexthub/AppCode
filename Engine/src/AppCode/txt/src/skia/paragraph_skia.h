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

#ifndef APPCODE_TXT_SRC_SKIA_PARAGRAPH_SKIA_H_
#define APPCODE_TXT_SRC_SKIA_PARAGRAPH_SKIA_H_

#include <optional>

#include "txt/paragraph.h"

#include "third_party/skia/modules/skparagraph/include/Paragraph.h"

namespace txt {

// Implementation of Paragraph based on Skia's text layout module.
class ParagraphSkia : public Paragraph {
 public:
  ParagraphSkia(std::unique_ptr<skia::textlayout::Paragraph> paragraph,
                std::vector<appcode::DlPaint>&& dl_paints,
                bool impeller_enabled);

  virtual ~ParagraphSkia() = default;

  double GetMaxWidth() override;

  double GetHeight() override;

  double GetLongestLine() override;

  double GetMinIntrinsicWidth() override;

  double GetMaxIntrinsicWidth() override;

  double GetAlphabeticBaseline() override;

  double GetIdeographicBaseline() override;

  std::vector<LineMetrics>& GetLineMetrics() override;

  bool GetLineMetricsAt(
      int lineNumber,
      skia::textlayout::LineMetrics* lineMetrics) const override;

  size_t GetNumberOfLines() const override;

  int GetLineNumberAt(size_t utf16Offset) const override;

  bool DidExceedMaxLines() override;

  void Layout(double width) override;

  bool Paint(appcode::DisplayListBuilder* builder, double x, double y) override;

  std::vector<TextBox> GetRectsForRange(
      size_t start,
      size_t end,
      RectHeightStyle rect_height_style,
      RectWidthStyle rect_width_style) override;

  std::vector<TextBox> GetRectsForPlaceholders() override;

  PositionWithAffinity GetGlyphPositionAtCoordinate(double dx,
                                                    double dy) override;

  bool GetGlyphInfoAt(
      unsigned offset,
      skia::textlayout::Paragraph::GlyphInfo* glyphInfo) const override;

  bool GetClosestGlyphInfoAtCoordinate(
      double dx,
      double dy,
      skia::textlayout::Paragraph::GlyphInfo* glyphInfo) const override;

  Range<size_t> GetWordBoundary(size_t offset) override;

 private:
  TextStyle SkiaToTxt(const skia::textlayout::TextStyle& skia);

  std::unique_ptr<skia::textlayout::Paragraph> paragraph_;
  std::vector<appcode::DlPaint> dl_paints_;
  std::optional<std::vector<LineMetrics>> line_metrics_;
  std::vector<TextStyle> line_metrics_styles_;
  const bool impeller_enabled_;
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_SKIA_PARAGRAPH_SKIA_H_
