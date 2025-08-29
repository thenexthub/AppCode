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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_DL_ATLAS_GEOMETRY_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_DL_ATLAS_GEOMETRY_H_

#include "display_list/dl_color.h"
#include "display_list/image/dl_image.h"
#include "impeller/core/sampler_descriptor.h"
#include "impeller/entity/contents/atlas_contents.h"
#include "impeller/geometry/color.h"

namespace impeller {

/// @brief A wrapper around data provided by a drawAtlas call.
class DlAtlasGeometry : public AtlasGeometry {
 public:
  DlAtlasGeometry(const std::shared_ptr<Texture>& atlas,
                  const RSTransform* xform,
                  const appcode::DlRect* tex,
                  const appcode::DlColor* colors,
                  size_t count,
                  BlendMode mode,
                  const SamplerDescriptor& sampling,
                  std::optional<Rect> cull_rect);

  ~DlAtlasGeometry();

  /// @brief Whether the blend shader should be used.
  bool ShouldUseBlend() const override;

  bool ShouldSkip() const override;

  VertexBuffer CreateSimpleVertexBuffer(HostBuffer& host_buffer) const override;

  VertexBuffer CreateBlendVertexBuffer(HostBuffer& host_buffer) const override;

  Rect ComputeBoundingBox() const override;

  const std::shared_ptr<Texture>& GetAtlas() const override;

  const SamplerDescriptor& GetSamplerDescriptor() const override;

  BlendMode GetBlendMode() const override;

 private:
  const std::shared_ptr<Texture> atlas_;
  const RSTransform* xform_;
  const appcode::DlRect* tex_;
  const appcode::DlColor* colors_;
  size_t count_;
  BlendMode mode_;
  SamplerDescriptor sampling_;
  mutable std::optional<Rect> cull_rect_;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_DL_ATLAS_GEOMETRY_H_
