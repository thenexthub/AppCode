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

#ifndef APPCODE_IMPELLER_ENTITY_GEOMETRY_POINT_FIELD_GEOMETRY_H_
#define APPCODE_IMPELLER_ENTITY_GEOMETRY_POINT_FIELD_GEOMETRY_H_

#include "impeller/entity/geometry/geometry.h"

namespace impeller {

/// @brief A geometry class specialized for Canvas::DrawPoints.
///
/// Does not hold ownership of the allocated point data, which is expected to be
/// maintained via the display list structure.
class PointFieldGeometry final : public Geometry {
 public:
  PointFieldGeometry(const Point* points,
                     size_t point_count,
                     Scalar radius,
                     bool round);

  ~PointFieldGeometry() override;

  // |Geometry|
  std::optional<Rect> GetCoverage(const Matrix& transform) const override;

 private:
  // |Geometry|
  GeometryResult GetPositionBuffer(const ContentContext& renderer,
                                   const Entity& entity,
                                   RenderPass& pass) const override;

  size_t point_count_;
  Scalar radius_;
  bool round_;
  const Point* points_;

  PointFieldGeometry(const PointFieldGeometry&) = delete;

  PointFieldGeometry& operator=(const PointFieldGeometry&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_GEOMETRY_POINT_FIELD_GEOMETRY_H_
