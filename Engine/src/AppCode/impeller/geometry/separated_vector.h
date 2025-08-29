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

#ifndef APPCODE_IMPELLER_GEOMETRY_SEPARATED_VECTOR_H_
#define APPCODE_IMPELLER_GEOMETRY_SEPARATED_VECTOR_H_

#include "impeller/geometry/point.h"

#include "impeller/geometry/scalar.h"

namespace impeller {

/// @brief  A Vector2, broken down as a separate magnitude and direction.
///         Assumes that the direction given is normalized.
///
///         This is a simple convenience struct for handling polyline offset
///         values when generating stroke geometry. For performance reasons,
///         it's sometimes adventageous to track the direction and magnitude
///         for offsets separately.
struct SeparatedVector2 {
  /// The normalized direction of the vector.
  Vector2 direction;

  /// The magnitude of the vector.
  Scalar magnitude = 0.0;

  SeparatedVector2();
  SeparatedVector2(Vector2 direction, Scalar magnitude);
  explicit SeparatedVector2(Vector2 vector);

  /// Returns the vector representation of the vector.
  Vector2 GetVector() const;

  /// Returns just the normalized direction vector.
  Vector2 GetDirection() const;

  /// Returns the scalar alignment of the two vectors.
  /// In other words, the dot product of the two normalized vectors.
  ///
  /// Range: [-1, 1]
  /// A value of 1 indicates the directions are parallel and pointing in the
  /// same direction. A value of -1 indicates the vectors are parallel and
  /// pointing in opposite directions. A value of 0 indicates the vectors are
  /// perpendicular.
  Scalar GetAlignment(const SeparatedVector2& other) const;

  /// Returns the scalar angle between the two rays.
  Radians AngleTo(const SeparatedVector2& other) const;

  /// Returns the cross product of the two direction vectors.
  Scalar Cross(const SeparatedVector2& other) const;
};

#endif  // APPCODE_IMPELLER_GEOMETRY_SEPARATED_VECTOR_H_

}  // namespace impeller
