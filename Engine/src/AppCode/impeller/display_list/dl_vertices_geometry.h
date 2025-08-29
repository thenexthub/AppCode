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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_DL_VERTICES_GEOMETRY_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_DL_VERTICES_GEOMETRY_H_

#include "appcode/display_list/dl_vertices.h"
#include "impeller/core/formats.h"
#include "impeller/entity/geometry/vertices_geometry.h"

namespace impeller {

/// @brief A geometry that is created from a DlVertices object.
class DlVerticesGeometry final : public VerticesGeometry {
 public:
  DlVerticesGeometry(const std::shared_ptr<const appcode::DlVertices>& vertices,
                     const ContentContext& renderer);

  ~DlVerticesGeometry() = default;

  GeometryResult GetPositionUVColorBuffer(Rect texture_coverage,
                                          Matrix effect_transform,
                                          const ContentContext& renderer,
                                          const Entity& entity,
                                          RenderPass& pass) const override;

  // |Geometry|
  GeometryResult GetPositionBuffer(const ContentContext& renderer,
                                   const Entity& entity,
                                   RenderPass& pass) const override;

  // |Geometry|
  std::optional<Rect> GetCoverage(const Matrix& transform) const override;

  bool HasVertexColors() const override;

  bool HasTextureCoordinates() const override;

  std::optional<Rect> GetTextureCoordinateCoverage() const override;

 private:
  PrimitiveType GetPrimitiveType() const;

  /// @brief Check if index normalization is required, returning whether or
  ///        not it was performed.
  ///
  /// If true, [indices_] should be used in place of the vertices object's
  /// indices.
  bool MaybePerformIndexNormalization(const ContentContext& renderer);

  const std::shared_ptr<const appcode::DlVertices> vertices_;
  std::vector<uint16_t> indices_;
  bool performed_normalization_ = false;
  Rect bounds_;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_DL_VERTICES_GEOMETRY_H_
