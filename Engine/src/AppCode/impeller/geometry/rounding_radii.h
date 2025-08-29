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

#ifndef APPCODE_IMPELLER_GEOMETRY_ROUNDING_RADII_H_
#define APPCODE_IMPELLER_GEOMETRY_ROUNDING_RADII_H_

#include "appcode/impeller/geometry/point.h"
#include "appcode/impeller/geometry/rect.h"
#include "appcode/impeller/geometry/size.h"

namespace impeller {

struct RoundingRadii {
  Size top_left;
  Size top_right;
  Size bottom_left;
  Size bottom_right;

  constexpr static RoundingRadii MakeRadius(Scalar radius) {
    return {Size(radius), Size(radius), Size(radius), Size(radius)};
  }

  constexpr static RoundingRadii MakeRadii(Size radii) {
    return {radii, radii, radii, radii};
  }

  constexpr static RoundingRadii MakeNinePatch(Scalar left,
                                               Scalar top,
                                               Scalar right,
                                               Scalar bottom) {
    return {
        .top_left = Size{left, top},
        .top_right = Size{right, top},
        .bottom_left = Size(left, bottom),
        .bottom_right = Size(right, bottom),
    };
  }

  constexpr bool IsFinite() const {
    return top_left.IsFinite() &&     //
           top_right.IsFinite() &&    //
           bottom_left.IsFinite() &&  //
           bottom_right.IsFinite();
  }

  constexpr bool AreAllCornersEmpty() const {
    return top_left.IsEmpty() &&     //
           top_right.IsEmpty() &&    //
           bottom_left.IsEmpty() &&  //
           bottom_right.IsEmpty();
  }

  constexpr bool AreAllCornersSame(Scalar tolerance = kEhCloseEnough) const {
    return ScalarNearlyEqual(top_left.width, top_right.width, tolerance) &&
           ScalarNearlyEqual(top_left.width, bottom_right.width, tolerance) &&
           ScalarNearlyEqual(top_left.width, bottom_left.width, tolerance) &&
           ScalarNearlyEqual(top_left.height, top_right.height, tolerance) &&
           ScalarNearlyEqual(top_left.height, bottom_right.height, tolerance) &&
           ScalarNearlyEqual(top_left.height, bottom_left.height, tolerance);
  }

  /// @brief  Returns a scaled copy of this object, ensuring that the sum of the
  ///         corner radii on each side does not exceed the width or height of
  ///         the given bounds.
  ///
  ///         See the [Skia scaling
  ///         implementation](https://github.com/google/skia/blob/main/src/core/SkRRect.cpp)
  ///         for more details.
  RoundingRadii Scaled(const Rect& bounds) const;

  constexpr inline RoundingRadii operator*(Scalar scale) {
    return {
        .top_left = top_left * scale,
        .top_right = top_right * scale,
        .bottom_left = bottom_left * scale,
        .bottom_right = bottom_right * scale,
    };
  }

  [[nodiscard]] constexpr bool operator==(const RoundingRadii& rr) const {
    return top_left == rr.top_left &&        //
           top_right == rr.top_right &&      //
           bottom_left == rr.bottom_left &&  //
           bottom_right == rr.bottom_right;
  }

  [[nodiscard]] constexpr bool operator!=(const RoundingRadii& rr) const {
    return !(*this == rr);
  }
};

}  // namespace impeller

namespace std {

inline std::ostream& operator<<(std::ostream& out,
                                const impeller::RoundingRadii& rr) {
  out << "("                               //
      << "ul: " << rr.top_left << ", "     //
      << "ur: " << rr.top_right << ", "    //
      << "ll: " << rr.bottom_left << ", "  //
      << "lr: " << rr.bottom_right         //
      << ")";
  return out;
}

}  // namespace std

#endif  // APPCODE_IMPELLER_GEOMETRY_ROUNDING_RADII_H_
