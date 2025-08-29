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

#ifndef APPCODE_IMPELLER_GEOMETRY_RSTRANSFORM_H_
#define APPCODE_IMPELLER_GEOMETRY_RSTRANSFORM_H_

#include "appcode/impeller/geometry/point.h"
#include "appcode/impeller/geometry/rect.h"
#include "appcode/impeller/geometry/scalar.h"
#include "appcode/impeller/geometry/size.h"

namespace impeller {

/// A utility struct that stores a simple transform composed of a translation,
/// a rotation, and a uniform scale.
///
/// This transform is used by drawAtlas to transform the sprites.
/// This structure mirrors the Flutter RSTransform class.
struct RSTransform {
  constexpr RSTransform()
      : scaled_cos(1.0f),
        scaled_sin(0.0f),
        translate_x(0.0f),
        translate_y(0.0f) {}

  constexpr RSTransform(Scalar scaled_cos,
                        Scalar scaled_sin,
                        Scalar translate_x,
                        Scalar translate_y)
      : scaled_cos(scaled_cos),
        scaled_sin(scaled_sin),
        translate_x(translate_x),
        translate_y(translate_y) {}

  /// Constructs an RSTransform from the indicated origin, uniform scale,
  /// and radians rotation.
  static constexpr RSTransform Make(Point origin,
                                    Scalar scale,
                                    Radians radians) {
    auto scaled_cos_sin = Matrix::CosSin(radians) * scale;
    return {scaled_cos_sin.x, scaled_cos_sin.y, origin.x, origin.y};
  }

  Scalar scaled_cos;
  Scalar scaled_sin;
  Scalar translate_x;
  Scalar translate_y;

  /// Returns true iff the resulting transformed quad will be aligned with
  /// the axes, even if rotated by a quadrant rotation.
  bool IsAxisAligned() const;

  Matrix GetMatrix() const;

  /// Returns the 4 corner points of the transformed quad for a sub-image
  /// of the indicated size in the same order as Rect::GetPoints.
  ///
  /// The order is UpperLeft, UpperRight, LowerLeft, LowerRight
  void GetQuad(Scalar width, Scalar height, Quad& quad) const;
  Quad GetQuad(Scalar width, Scalar height) const;
  Quad GetQuad(Size size) const;

  /// Returns the bounds of the 4 corner points of the transformed quad
  /// for a sub-image of the indicated size.
  std::optional<Rect> GetBounds(Scalar width, Scalar height) const;
  std::optional<Rect> GetBounds(Size size) const;
};

}  // namespace impeller

namespace std {

inline std::ostream& operator<<(std::ostream& out,
                                const impeller::RSTransform& rst) {
  // clang-format off
  out << "("
      << "scos: " << rst.scaled_cos << ", "
      << "ssin: " << rst.scaled_sin << ", "
      << "origin: (" << rst.translate_x << ", "
                     << rst.translate_y << ")"
      << ")";
  // clang-format on
  return out;
}

}  // namespace std

#endif  // APPCODE_IMPELLER_GEOMETRY_RSTRANSFORM_H_
