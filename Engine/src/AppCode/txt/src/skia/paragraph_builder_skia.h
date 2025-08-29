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

#ifndef APPCODE_TXT_SRC_SKIA_PARAGRAPH_BUILDER_SKIA_H_
#define APPCODE_TXT_SRC_SKIA_PARAGRAPH_BUILDER_SKIA_H_

#include "txt/paragraph_builder.h"

#include "appcode/display_list/dl_paint.h"
#include "third_party/skia/modules/skparagraph/include/ParagraphBuilder.h"

namespace txt {

//------------------------------------------------------------------------------
/// @brief      ParagraphBuilder implementation using Skia's text layout module.
///
/// @note       Despite the suffix "Skia", this class is not specific to Skia
///             and is also used with the Impeller backend.
class ParagraphBuilderSkia : public ParagraphBuilder {
 public:
  ParagraphBuilderSkia(const ParagraphStyle& style,
                       const std::shared_ptr<FontCollection>& font_collection,
                       const bool impeller_enabled);

  virtual ~ParagraphBuilderSkia();

  virtual void PushStyle(const TextStyle& style) override;
  virtual void Pop() override;
  virtual const TextStyle& PeekStyle() override;
  virtual void AddText(const std::u16string& text) override;
  virtual void AddText(const uint8_t* utf8_data, size_t byte_length) override;
  virtual void AddPlaceholder(PlaceholderRun& span) override;
  virtual std::unique_ptr<Paragraph> Build() override;

 private:
  friend class SkiaParagraphBuilderTests_ParagraphStrutStyle_Test;

  skia::textlayout::ParagraphPainter::PaintID CreatePaintID(
      const appcode::DlPaint& dl_paint);
  skia::textlayout::ParagraphStyle TxtToSkia(const ParagraphStyle& txt);
  skia::textlayout::TextStyle TxtToSkia(const TextStyle& txt);

  std::shared_ptr<skia::textlayout::ParagraphBuilder> builder_;
  TextStyle base_style_;

  /// @brief      Whether Impeller is enabled in the runtime.
  ///
  /// @note       As of the time of this writing, this is used to draw text
  ///             decorations (i.e. dashed and dotted lines) directly using the
  ///             `drawLine` API, because Impeller's path rendering does not
  ///             support dashed and dotted lines (but Skia's does).
  const bool impeller_enabled_;
  std::stack<TextStyle> txt_style_stack_;
  std::vector<appcode::DlPaint> dl_paints_;
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_SKIA_PARAGRAPH_BUILDER_SKIA_H_
