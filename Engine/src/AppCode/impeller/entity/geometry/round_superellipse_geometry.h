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

#ifndef APPCODE_IMPELLER_ENTITY_GEOMETRY_ROUND_SUPERELLIPSE_GEOMETRY_H_
#define APPCODE_IMPELLER_ENTITY_GEOMETRY_ROUND_SUPERELLIPSE_GEOMETRY_H_

#include "impeller/entity/geometry/geometry.h"
#include "impeller/geometry/rounding_radii.h"

namespace impeller {

/// Geometry class that can generate vertices for a rounded superellipse.
///
/// A rounded superellipse is a shape similar to a typical rounded rectangle
/// (`RoundRect`), but with smoother transitions between the straight sides and
/// the rounded corners. It resembles the `RoundedRectangle` shape in SwiftUI
/// with the `.continuous` corner style. Technically, it is created by replacing
/// the four corners of a superellipse (also known as a Lamé curve) with
/// circular arcs.
///
/// The `bounds` defines the position and size of the shape. The `corner_radius`
/// corresponds to SwiftUI's `cornerRadius` parameter, which is close to, but
/// not exactly equals to, the radius of the corner circles.
class RoundSuperellipseGeometry final : public Geometry {
 public:
  RoundSuperellipseGeometry(const Rect& bounds, const RoundingRadii& radii);
  RoundSuperellipseGeometry(const Rect& bounds, float corner_radius);

  ~RoundSuperellipseGeometry() override;

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

  const Rect bounds_;
  const RoundingRadii radii_;

  RoundSuperellipseGeometry(const RoundSuperellipseGeometry&) = delete;

  RoundSuperellipseGeometry& operator=(const RoundSuperellipseGeometry&) =
      delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_GEOMETRY_ROUND_SUPERELLIPSE_GEOMETRY_H_
