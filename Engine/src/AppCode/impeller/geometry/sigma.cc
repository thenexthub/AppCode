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

#include "impeller/geometry/sigma.h"

#include <sstream>

namespace impeller {

Sigma::operator Radius() const {
  return Radius{sigma > 0.5f ? (sigma - 0.5f) * kKernelRadiusPerSigma : 0.0f};
}

Radius::operator Sigma() const {
  return Sigma{radius > 0 ? radius / kKernelRadiusPerSigma + 0.5f : 0.0f};
}

}  // namespace impeller
