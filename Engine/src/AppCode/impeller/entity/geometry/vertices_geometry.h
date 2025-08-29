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

#ifndef APPCODE_IMPELLER_ENTITY_GEOMETRY_VERTICES_GEOMETRY_H_
#define APPCODE_IMPELLER_ENTITY_GEOMETRY_VERTICES_GEOMETRY_H_

#include "impeller/entity/geometry/geometry.h"

namespace impeller {

/// @brief A geometry that is created from a vertices object.
class VerticesGeometry : public Geometry {
 public:
  virtual GeometryResult GetPositionUVColorBuffer(
      Rect texture_coverage,
      Matrix effect_transform,
      const ContentContext& renderer,
      const Entity& entity,
      RenderPass& pass) const = 0;

  virtual bool HasVertexColors() const = 0;

  virtual bool HasTextureCoordinates() const = 0;

  virtual std::optional<Rect> GetTextureCoordinateCoverage() const = 0;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_GEOMETRY_VERTICES_GEOMETRY_H_
