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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_CONICAL_GRADIENT_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_CONICAL_GRADIENT_CONTENTS_H_

#include <vector>

#include "impeller/entity/contents/color_source_contents.h"
#include "impeller/entity/entity.h"
#include "impeller/geometry/color.h"
#include "impeller/geometry/point.h"

namespace impeller {

class ConicalGradientContents final : public ColorSourceContents {
 public:
  ConicalGradientContents();

  ~ConicalGradientContents() override;

  // |Contents|
  bool Render(const ContentContext& renderer,
              const Entity& entity,
              RenderPass& pass) const override;

  // |Contents|
  [[nodiscard]] bool ApplyColorFilter(
      const ColorFilterProc& color_filter_proc) override;

  void SetCenterAndRadius(Point center, Scalar radius);

  void SetColors(std::vector<Color> colors);

  void SetStops(std::vector<Scalar> stops);

  const std::vector<Color>& GetColors() const;

  const std::vector<Scalar>& GetStops() const;

  void SetTileMode(Entity::TileMode tile_mode);

  void SetFocus(std::optional<Point> focus, Scalar radius);

 private:
  bool RenderTexture(const ContentContext& renderer,
                     const Entity& entity,
                     RenderPass& pass) const;

  bool RenderSSBO(const ContentContext& renderer,
                  const Entity& entity,
                  RenderPass& pass) const;

  bool RenderUniform(const ContentContext& renderer,
                     const Entity& entity,
                     RenderPass& pass) const;

  Point center_;
  Scalar radius_ = 0.0f;
  std::vector<Color> colors_;
  std::vector<Scalar> stops_;
  Entity::TileMode tile_mode_;
  Color decal_border_color_ = Color::BlackTransparent();
  std::optional<Point> focus_;
  Scalar focus_radius_ = 0.0f;

  ConicalGradientContents(const ConicalGradientContents&) = delete;

  ConicalGradientContents& operator=(const ConicalGradientContents&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_CONICAL_GRADIENT_CONTENTS_H_
