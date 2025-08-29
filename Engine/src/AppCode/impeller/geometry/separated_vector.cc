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

#include "separated_vector.h"

namespace impeller {

SeparatedVector2::SeparatedVector2() = default;

SeparatedVector2::SeparatedVector2(Vector2 direction, Scalar magnitude)
    : direction(direction), magnitude(magnitude){};

SeparatedVector2::SeparatedVector2(Vector2 vector)
    : direction(vector.Normalize()), magnitude(vector.GetLength()){};

Vector2 SeparatedVector2::GetVector() const {
  return direction * magnitude;
}

Vector2 SeparatedVector2::GetDirection() const {
  return direction;
}

Scalar SeparatedVector2::GetAlignment(const SeparatedVector2& other) const {
  return direction.Dot(other.direction);
}

Radians SeparatedVector2::AngleTo(const SeparatedVector2& other) const {
  return direction.AngleTo(other.direction);
}

Scalar SeparatedVector2::Cross(const SeparatedVector2& other) const {
  return direction.Cross(other.direction);
}

}  // namespace impeller
