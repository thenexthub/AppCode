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

#ifndef APPCODE_IMPELLER_ENTITY_GEOMETRY_SUPERELLIPSE_GEOMETRY_H_
#define APPCODE_IMPELLER_ENTITY_GEOMETRY_SUPERELLIPSE_GEOMETRY_H_

#include "impeller/entity/geometry/geometry.h"

namespace impeller {

/// Geometry class that can generate vertices for a superellipse.
///
/// A Superellipse is an ellipse-like shape that is defined by the parameters N,
/// alpha, and beta:
///
///  1 = |x / b| ^n + |y / a| ^n
///
/// The radius and center apply a uniform scaling and offset that is separate
/// from alpha or beta. When n = 4, the shape is referred to as a rectellipse.
///
/// See also: https://en.wikipedia.org/wiki/Superellipse
class SuperellipseGeometry final : public Geometry {
 public:
  explicit SuperellipseGeometry(const Point& center,
                                Scalar radius,
                                Scalar degree,
                                Scalar alpha,
                                Scalar beta);

  ~SuperellipseGeometry() override;

  // |Geometry|
  bool CoversArea(const Matrix& transform, const Rect& rect) const override;

  // |Geometry|
  bool IsAxisAlignedRect() const override;

 private:
  // |Geometry|
  GeometryResult GetPositionBuffer(const ContentContext& renderer,
                                   const Entity& entity,
                                   RenderPass& pass) const override;

  // |Geometry|
  std::optional<Rect> GetCoverage(const Matrix& transform) const override;

  Point center_;
  // 4 is a rectellipse
  Scalar degree_;
  Scalar radius_;
  Scalar alpha_;
  Scalar beta_;

  SuperellipseGeometry(const SuperellipseGeometry&) = delete;

  SuperellipseGeometry& operator=(const SuperellipseGeometry&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_GEOMETRY_SUPERELLIPSE_GEOMETRY_H_
