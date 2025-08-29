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

#ifndef APPCODE_IMPELLER_ENTITY_GEOMETRY_CIRCLE_GEOMETRY_H_
#define APPCODE_IMPELLER_ENTITY_GEOMETRY_CIRCLE_GEOMETRY_H_

#include "impeller/entity/geometry/geometry.h"

namespace impeller {

// Geometry class that can generate vertices (with or without texture
// coordinates) for either filled or stroked circles
class CircleGeometry final : public Geometry {
 public:
  explicit CircleGeometry(const Point& center, Scalar radius);

  explicit CircleGeometry(const Point& center,
                          Scalar radius,
                          Scalar stroke_width);

  ~CircleGeometry() override;

  // |Geometry|
  bool CoversArea(const Matrix& transform, const Rect& rect) const override;

  // |Geometry|
  bool IsAxisAlignedRect() const override;

  // |Geometry|
  Scalar ComputeAlphaCoverage(const Matrix& transform) const override;

 private:
  // |Geometry|
  GeometryResult GetPositionBuffer(const ContentContext& renderer,
                                   const Entity& entity,
                                   RenderPass& pass) const override;

  // |Geometry|
  std::optional<Rect> GetCoverage(const Matrix& transform) const override;

  Point center_;
  Scalar radius_;
  Scalar stroke_width_;

  CircleGeometry(const CircleGeometry&) = delete;

  CircleGeometry& operator=(const CircleGeometry&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_GEOMETRY_CIRCLE_GEOMETRY_H_
