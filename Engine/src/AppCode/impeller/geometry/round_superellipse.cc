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

#include "appcode/impeller/geometry/round_superellipse.h"

#include "appcode/impeller/geometry/round_rect.h"
#include "appcode/impeller/geometry/round_superellipse_param.h"

namespace impeller {

RoundSuperellipse RoundSuperellipse::MakeRectRadii(
    const Rect& in_bounds,
    const RoundingRadii& in_radii) {
  if (!in_bounds.IsFinite()) {
    return {};
  }
  Rect bounds = in_bounds.GetPositive();
  // RoundingRadii::Scaled might return an empty radii if bounds or in_radii is
  // empty, which is expected. Pass along the bounds even if the radii is empty
  // as it would still have a valid location and/or 1-dimensional size which
  // might appear when stroked
  return RoundSuperellipse(bounds, in_radii.Scaled(bounds));
}

[[nodiscard]] bool RoundSuperellipse::Contains(const Point& p) const {
  if (!bounds_.Contains(p)) {
    return false;
  }
  auto param = RoundSuperellipseParam::MakeBoundsRadii(bounds_, radii_);
  return param.Contains(p);
}

RoundRect RoundSuperellipse::ToApproximateRoundRect() const {
  // Experiments have shown that using the same corner radii for the RRect
  // provides an approximation that is close to optimal, as achieving a perfect
  // match is not feasible.
  return RoundRect::MakeRectRadii(GetBounds(), GetRadii());
}

}  // namespace impeller
