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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_VERTICES_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_VERTICES_CONTENTS_H_

#include <memory>

#include "impeller/core/sampler_descriptor.h"
#include "impeller/entity/contents/contents.h"
#include "impeller/entity/entity.h"
#include "impeller/entity/geometry/vertices_geometry.h"
#include "impeller/geometry/color.h"

namespace impeller {

/// A vertices contents for (optional) per-color vertices + texture and any
/// blend mode.
class VerticesSimpleBlendContents final : public Contents {
 public:
  VerticesSimpleBlendContents();

  ~VerticesSimpleBlendContents() override;

  using LazyTexture =
      std::function<std::shared_ptr<Texture>(const ContentContext& renderer)>;

  void SetGeometry(std::shared_ptr<VerticesGeometry> geometry);

  void SetAlpha(Scalar alpha);

  void SetBlendMode(BlendMode blend_mode);

  void SetTexture(std::shared_ptr<Texture> texture);

  void SetLazyTexture(const LazyTexture& lazy_texture);

  void SetSamplerDescriptor(const SamplerDescriptor& descriptor);

  void SetTileMode(Entity::TileMode tile_mode_x, Entity::TileMode tile_mode_y);

  void SetEffectTransform(Matrix transform);

  void SetLazyTextureCoverage(Rect rect);

  // |Contents|
  std::optional<Rect> GetCoverage(const Entity& entity) const override;

  // |Contents|
  bool Render(const ContentContext& renderer,
              const Entity& entity,
              RenderPass& pass) const override;

 private:
  Scalar alpha_ = 1.0;
  std::shared_ptr<VerticesGeometry> geometry_;
  std::shared_ptr<Texture> texture_;
  BlendMode blend_mode_ = BlendMode::kSrc;
  SamplerDescriptor descriptor_ = {};
  Entity::TileMode tile_mode_x_ = Entity::TileMode::kClamp;
  Entity::TileMode tile_mode_y_ = Entity::TileMode::kClamp;
  Matrix inverse_matrix_ = {};
  std::optional<Rect> lazy_texture_coverage_;
  LazyTexture lazy_texture_;

  VerticesSimpleBlendContents(const VerticesSimpleBlendContents&) = delete;

  VerticesSimpleBlendContents& operator=(const VerticesSimpleBlendContents&) =
      delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_VERTICES_CONTENTS_H_
