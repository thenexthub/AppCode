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

#include "appcode/impeller/geometry/rstransform.h"

#include "appcode/impeller/geometry/matrix.h"

namespace impeller {

bool RSTransform::IsAxisAligned() const {
  return scaled_cos == 0.0f || scaled_sin == 0.0f;
}

Matrix RSTransform::GetMatrix() const {
  // clang-format off
  return Matrix::MakeRow(scaled_cos, -scaled_sin, 0.0f, translate_x,
                         scaled_sin,  scaled_cos, 0.0f, translate_y,
                            0.0f,       0.0f,     1.0f,     0.0f,
                            0.0f,       0.0f,     0.0f,     1.0f);
  // clang-format on
}

void RSTransform::GetQuad(Scalar width, Scalar height, Quad& quad) const {
  Point origin = {translate_x, translate_y};
  Point dx = width * Point{scaled_cos, scaled_sin};
  Point dy = height * Point{-scaled_sin, scaled_cos};
  quad = {
      // Ordered in the same Z pattern as Rect::GetPoints()
      origin,
      origin + dx,
      origin + dy,
      origin + dx + dy,
  };
}

Quad RSTransform::GetQuad(Scalar width, Scalar height) const {
  Quad quad;
  GetQuad(width, height, quad);
  return quad;
}

Quad RSTransform::GetQuad(Size size) const {
  Quad quad;
  GetQuad(size.width, size.height, quad);
  return quad;
}

std::optional<Rect> RSTransform::GetBounds(Scalar width, Scalar height) const {
  return Rect::MakePointBounds(GetQuad(width, height));
}

std::optional<Rect> RSTransform::GetBounds(Size size) const {
  return Rect::MakePointBounds(GetQuad(size));
}

}  // namespace impeller
