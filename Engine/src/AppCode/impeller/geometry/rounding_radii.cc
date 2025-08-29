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

#include "appcode/impeller/geometry/rounding_radii.h"

namespace impeller {

static inline void NormalizeEmptyToZero(Size& radii) {
  if (radii.IsEmpty()) {
    radii = Size();
  }
}

static inline void AdjustScale(Scalar& radius1,
                               Scalar& radius2,
                               Scalar dimension,
                               Scalar& scale) {
  FML_DCHECK(radius1 >= 0.0f && radius2 >= 0.0f);
  FML_DCHECK(dimension > 0.0f);
  if (radius1 + radius2 > dimension) {
    scale = std::min(scale, dimension / (radius1 + radius2));
  }
}

RoundingRadii RoundingRadii::Scaled(const Rect& in_bounds) const {
  Rect bounds = in_bounds.GetPositive();
  if (bounds.IsEmpty() ||  //
      AreAllCornersEmpty() || !IsFinite()) {
    // Normalize empty radii.
    return RoundingRadii();
  }

  // Copy the incoming radii so that we can work on normalizing them to the
  // particular rectangle they are paired with without disturbing the caller.
  RoundingRadii radii = *this;

  // If any corner is flat or has a negative value, normalize it to zeros
  // We do this first so that the unnecessary non-flat part of that radius
  // does not contribute to the global scaling below.
  NormalizeEmptyToZero(radii.top_left);
  NormalizeEmptyToZero(radii.top_right);
  NormalizeEmptyToZero(radii.bottom_left);
  NormalizeEmptyToZero(radii.bottom_right);

  // Now determine a global scale to apply to all of the radii to ensure
  // that none of the adjacent pairs of radius values sum to larger than
  // the corresponding dimension of the rectangle.
  Size size = bounds.GetSize();
  Scalar scale = 1.0f;
  // clang-format off
  AdjustScale(radii.top_left.width,    radii.top_right.width,     size.width,
              scale);
  AdjustScale(radii.bottom_left.width, radii.bottom_right.width,  size.width,
              scale);
  AdjustScale(radii.top_left.height,   radii.bottom_left.height,  size.height,
              scale);
  AdjustScale(radii.top_right.height,  radii.bottom_right.height, size.height,
              scale);
  // clang-format on
  if (scale < 1.0f) {
    radii = radii * scale;
  }

  return radii;
}

}  // namespace impeller
