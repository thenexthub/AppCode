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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_PARAGRAPH_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_PARAGRAPH_H_

#include "appcode/txt/src/txt/paragraph.h"
#include "impeller/toolkit/interop/glyph_info.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/line_metrics.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

/**
 * An immutable fully laid out paragraph.
 */
class Paragraph final
    : public Object<Paragraph,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerParagraph)> {
 public:
  explicit Paragraph(std::unique_ptr<txt::Paragraph> paragraph);

  ~Paragraph() override;

  Paragraph(const Paragraph&) = delete;

  Paragraph& operator=(const Paragraph&) = delete;

  Scalar GetMaxWidth() const;

  Scalar GetHeight() const;

  Scalar GetLongestLineWidth() const;

  Scalar GetMinIntrinsicWidth() const;

  Scalar GetMaxIntrinsicWidth() const;

  Scalar GetIdeographicBaseline() const;

  Scalar GetAlphabeticBaseline() const;

  uint32_t GetLineCount() const;

  const std::unique_ptr<txt::Paragraph>& GetHandle() const;

  ScopedObject<LineMetrics> GetLineMetrics() const;

  ScopedObject<GlyphInfo> GetGlyphInfoAtCodeUnitIndex(
      size_t code_unit_index) const;

  ScopedObject<GlyphInfo> GetClosestGlyphInfoAtParagraphCoordinates(
      double x,
      double y) const;

  ImpellerRange GetWordBoundary(size_t code_unit_index) const;

 private:
  std::unique_ptr<txt::Paragraph> paragraph_;
  mutable ScopedObject<LineMetrics> lazy_line_metrics_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_PARAGRAPH_H_
