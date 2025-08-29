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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_TEXT_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_TEXT_CONTENTS_H_

#include <memory>

#include "impeller/entity/contents/content_context.h"
#include "impeller/entity/contents/contents.h"
#include "impeller/entity/contents/pipelines.h"
#include "impeller/geometry/color.h"
#include "impeller/typographer/font_glyph_pair.h"
#include "impeller/typographer/text_frame.h"

namespace impeller {

class LazyGlyphAtlas;
class Context;

class TextContents final : public Contents {
 public:
  TextContents();

  ~TextContents();

  void SetTextFrame(const std::shared_ptr<TextFrame>& frame);

  void SetColor(Color color);

  /// @brief Force the text color to apply to the rendered glyphs, even if those
  ///        glyphs are bitmaps.
  ///
  ///        This is used to ensure that mask blurs work correctly on emoji.
  void SetForceTextColor(bool value);

  /// Must be set after text frame.
  void SetTextProperties(Color color,
                         bool stroke,
                         Scalar stroke_width,
                         Cap stroke_cap,
                         Join stroke_join,
                         Scalar stroke_miter);

  Color GetColor() const;

  // |Contents|
  void SetInheritedOpacity(Scalar opacity) override;

  // The offset is only used for computing the subpixel glyph position.
  void SetOffset(Vector2 offset);

  std::optional<Rect> GetTextFrameBounds() const;

  // |Contents|
  std::optional<Rect> GetCoverage(const Entity& entity) const override;

  void SetScale(Scalar scale) { scale_ = scale; }

  // |Contents|
  bool Render(const ContentContext& renderer,
              const Entity& entity,
              RenderPass& pass) const override;

  static void ComputeVertexData(
      GlyphAtlasPipeline::VertexShader::PerVertexData* vtx_contents,
      const std::shared_ptr<TextFrame>& frame,
      Scalar scale,
      const Matrix& entity_transform,
      Vector2 offset,
      std::optional<GlyphProperties> glyph_properties,
      const std::shared_ptr<GlyphAtlas>& atlas);

 private:
  std::optional<GlyphProperties> GetGlyphProperties() const;

  std::shared_ptr<TextFrame> frame_;
  Scalar scale_ = 1.0;
  Scalar inherited_opacity_ = 1.0;
  Vector2 offset_;
  bool force_text_color_ = false;
  Color color_;
  GlyphProperties properties_;

  TextContents(const TextContents&) = delete;

  TextContents& operator=(const TextContents&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_TEXT_CONTENTS_H_
