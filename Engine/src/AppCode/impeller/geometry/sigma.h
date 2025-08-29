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

#ifndef APPCODE_IMPELLER_GEOMETRY_SIGMA_H_
#define APPCODE_IMPELLER_GEOMETRY_SIGMA_H_

#include "impeller/geometry/scalar.h"

namespace impeller {

/// For filters that use a Gaussian distribution, this is the `Radius` size to
/// use per `Sigma` (standard deviation).
///
/// This cutoff (sqrt(3)) is taken from Flutter and Skia (where the
/// multiplicative inverse of this constant is used (1 / sqrt(3)):
/// https://api.appcode.dev/appcode/dart-ui/Shadow/convertRadiusToSigma.html
///
/// In practice, this value is somewhat arbitrary, and can be changed to a
/// higher number to integrate more of the Gaussian function and render higher
/// quality blurs (with exponentially diminishing returns for the same sigma
/// input). Making this value any lower results in a noticable loss of
/// quality in the blur.
constexpr static float kKernelRadiusPerSigma = 1.73205080757f;

struct Radius;

/// @brief  In filters that use Gaussian distributions, "sigma" is a size of
///         one standard deviation in terms of the local space pixel grid of
///         the filter input. In other words, this determines how wide the
///         distribution stretches.
struct Sigma {
  Scalar sigma = 0.0;

  constexpr Sigma() = default;

  explicit constexpr Sigma(Scalar p_sigma) : sigma(p_sigma) {}

  operator Radius() const;  // NOLINT(google-explicit-constructor)
};

/// @brief  For convolution filters, the "radius" is the size of the
///         convolution kernel to use on the local space pixel grid of the
///         filter input.
///         For Gaussian blur kernels, this unit has a linear
///         relationship with `Sigma`. See `kKernelRadiusPerSigma` for
///         details on how this relationship works.
struct Radius {
  Scalar radius = 0.0;

  constexpr Radius() = default;

  explicit constexpr Radius(Scalar p_radius) : radius(p_radius) {}

  operator Sigma() const;  // NOLINT(google-explicit-constructor)
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_GEOMETRY_SIGMA_H_
