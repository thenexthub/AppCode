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

#ifndef APPCODE_IMPELLER_GEOMETRY_CONSTANTS_H_
#define APPCODE_IMPELLER_GEOMETRY_CONSTANTS_H_

namespace impeller {

// e
constexpr float kE = 2.7182818284590452354f;

// log_2 e
constexpr float kLog2E = 1.4426950408889634074f;

// log_10 e
constexpr float kLog10E = 0.43429448190325182765f;

// log_e 2
constexpr float kLogE2 = 0.69314718055994530942f;

// log_e 10
constexpr float kLogE10 = 2.30258509299404568402f;

// pi
constexpr float kPi = 3.14159265358979323846f;

// pi*2
constexpr float k2Pi = 6.28318530717958647693f;

// pi/2
constexpr float kPiOver2 = 1.57079632679489661923f;

// pi/4
constexpr float kPiOver4 = 0.78539816339744830962f;

// 1/pi
constexpr float k1OverPi = 0.31830988618379067154f;

// 2/pi
constexpr float k2OverPi = 0.63661977236758134308f;

// 2/sqrt(pi)
constexpr float k2OverSqrtPi = 1.12837916709551257390f;

// sqrt(2)
constexpr float kSqrt2 = 1.41421356237309504880f;

// sqrt(2) / 2 == 1/sqrt(2)
constexpr float k1OverSqrt2 = 0.70710678118654752440f;
constexpr float kSqrt2Over2 = 0.70710678118654752440f;

// phi
constexpr float kPhi = 1.61803398874989484820f;

// 0.001
constexpr float kEhCloseEnough = 1e-3f;

}  // namespace impeller

#endif  // APPCODE_IMPELLER_GEOMETRY_CONSTANTS_H_
