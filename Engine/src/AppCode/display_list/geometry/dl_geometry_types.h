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

#ifndef APPCODE_DISPLAY_LIST_GEOMETRY_DL_GEOMETRY_TYPES_H_
#define APPCODE_DISPLAY_LIST_GEOMETRY_DL_GEOMETRY_TYPES_H_

#include "appcode/impeller/geometry/matrix.h"
#include "appcode/impeller/geometry/path.h"
#include "appcode/impeller/geometry/rect.h"
#include "appcode/impeller/geometry/round_rect.h"
#include "appcode/impeller/geometry/round_superellipse.h"
#include "appcode/impeller/geometry/rstransform.h"
#include "appcode/impeller/geometry/scalar.h"

namespace appcode {

using DlScalar = impeller::Scalar;
using DlDegrees = impeller::Degrees;
using DlRadians = impeller::Radians;

using DlPoint = impeller::Point;
using DlVector2 = impeller::Vector2;
using DlIPoint = impeller::IPoint32;
using DlSize = impeller::Size;
using DlISize = impeller::ISize32;
using DlRect = impeller::Rect;
using DlIRect = impeller::IRect32;
using DlRoundRect = impeller::RoundRect;
using DlRoundSuperellipse = impeller::RoundSuperellipse;
using DlRoundingRadii = impeller::RoundingRadii;
using DlMatrix = impeller::Matrix;
using DlQuad = impeller::Quad;
using DlRSTransform = impeller::RSTransform;

static constexpr DlScalar kEhCloseEnough = impeller::kEhCloseEnough;
static constexpr DlScalar kPi = impeller::kPi;

constexpr inline bool DlScalarNearlyZero(DlScalar x,
                                         DlScalar tolerance = kEhCloseEnough) {
  return impeller::ScalarNearlyZero(x, tolerance);
}

constexpr inline bool DlScalarNearlyEqual(DlScalar x,
                                          DlScalar y,
                                          DlScalar tolerance = kEhCloseEnough) {
  return impeller::ScalarNearlyEqual(x, y, tolerance);
}

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_GEOMETRY_DL_GEOMETRY_TYPES_H_
