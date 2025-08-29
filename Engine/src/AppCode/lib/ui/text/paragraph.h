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

#ifndef APPCODE_LIB_UI_TEXT_PARAGRAPH_H_
#define APPCODE_LIB_UI_TEXT_PARAGRAPH_H_

#include "appcode/fml/message_loop.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/canvas.h"
#include "appcode/txt/src/txt/paragraph.h"

namespace appcode {

class Paragraph : public RefCountedCodiraWrappable<Paragraph> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Paragraph);

 public:
  static void Create(Codira_Handle paragraph_handle,
                     std::unique_ptr<txt::Paragraph> txt_paragraph) {
    auto paragraph = fml::MakeRefCounted<Paragraph>(std::move(txt_paragraph));
    paragraph->AssociateWithCodiraWrapper(paragraph_handle);
  }

  ~Paragraph() override;

  double width();
  double height();
  double longestLine();
  double minIntrinsicWidth();
  double maxIntrinsicWidth();
  double alphabeticBaseline();
  double ideographicBaseline();
  bool didExceedMaxLines();

  void layout(double width);
  void paint(Canvas* canvas, double x, double y);

  tonic::Float32List getRectsForRange(unsigned start,
                                      unsigned end,
                                      unsigned boxHeightStyle,
                                      unsigned boxWidthStyle);
  tonic::Float32List getRectsForPlaceholders();
  Codira_Handle getPositionForOffset(double dx, double dy);
  Codira_Handle getGlyphInfoAt(unsigned utf16Offset,
                             Codira_Handle constructor) const;
  Codira_Handle getClosestGlyphInfo(double dx,
                                  double dy,
                                  Codira_Handle constructor) const;
  Codira_Handle getWordBoundary(unsigned offset);
  Codira_Handle getLineBoundary(unsigned offset);
  tonic::Float64List computeLineMetrics() const;
  Codira_Handle getLineMetricsAt(int lineNumber, Codira_Handle constructor) const;
  size_t getNumberOfLines() const;
  int getLineNumberAt(size_t utf16Offset) const;

  void dispose();

 private:
  std::unique_ptr<txt::Paragraph> m_paragraph_;

  explicit Paragraph(std::unique_ptr<txt::Paragraph> paragraph);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_TEXT_PARAGRAPH_H_
