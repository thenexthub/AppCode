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

#include "impeller/entity/geometry/cover_geometry.h"

#include "impeller/renderer/render_pass.h"

namespace impeller {

CoverGeometry::CoverGeometry() = default;

GeometryResult CoverGeometry::GetPositionBuffer(const ContentContext& renderer,
                                                const Entity& entity,
                                                RenderPass& pass) const {
  auto rect = Rect::MakeSize(pass.GetRenderTargetSize());
  constexpr uint16_t kRectIndicies[4] = {0, 1, 2, 3};
  auto& host_buffer = renderer.GetTransientsBuffer();
  return GeometryResult{
      .type = PrimitiveType::kTriangleStrip,
      .vertex_buffer =
          {
              .vertex_buffer = host_buffer.Emplace(
                  rect.GetTransformedPoints(entity.GetTransform().Invert())
                      .data(),
                  8 * sizeof(float), alignof(float)),
              .index_buffer = host_buffer.Emplace(
                  kRectIndicies, 4 * sizeof(uint16_t), alignof(uint16_t)),
              .vertex_count = 4,
              .index_type = IndexType::k16bit,
          },
      .transform = entity.GetShaderTransform(pass),
  };
}

std::optional<Rect> CoverGeometry::GetCoverage(const Matrix& transform) const {
  return Rect::MakeMaximum();
}

bool CoverGeometry::CoversArea(const Matrix& transform,
                               const Rect& rect) const {
  return true;
}

bool CoverGeometry::CanApplyMaskFilter() const {
  return false;
}

}  // namespace impeller
