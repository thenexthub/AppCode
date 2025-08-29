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

#ifndef APPCODE_IMPELLER_GEOMETRY_TRIG_H_
#define APPCODE_IMPELLER_GEOMETRY_TRIG_H_

#include <functional>
#include <vector>

#include "appcode/impeller/geometry/point.h"

namespace impeller {

/// @brief  A structure to store the sine and cosine of an angle.
struct Trig {
  /// Construct a Trig object from a given angle in radians.
  explicit Trig(Radians r)
      : cos(std::cos(r.radians)), sin(std::sin(r.radians)) {}

  /// Construct a Trig object from the given cosine and sine values.
  Trig(double cos, double sin) : cos(cos), sin(sin) {}

  double cos;
  double sin;

  /// @brief  Returns the vector rotated by the represented angle.
  Vector2 operator*(const Vector2& vector) const {
    return Vector2(static_cast<Scalar>(vector.x * cos - vector.y * sin),
                   static_cast<Scalar>(vector.x * sin + vector.y * cos));
  }

  /// @brief  Returns the Trig representing the negative version of this angle.
  Trig operator-() const { return Trig(cos, -sin); }

  /// @brief  Returns the corresponding point on a circle of a given |radius|.
  Vector2 operator*(double radius) const {
    return Vector2(static_cast<Scalar>(cos * radius),
                   static_cast<Scalar>(sin * radius));
  }

  /// @brief  Returns the corresponding point on an ellipse with the given size.
  Vector2 operator*(const Size& ellipse_radii) const {
    return Vector2(static_cast<Scalar>(cos * ellipse_radii.width),
                   static_cast<Scalar>(sin * ellipse_radii.height));
  }
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_GEOMETRY_TRIG_H_
