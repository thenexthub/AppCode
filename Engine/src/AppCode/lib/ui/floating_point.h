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

#ifndef APPCODE_LIB_UI_FLOATING_POINT_H_
#define APPCODE_LIB_UI_FLOATING_POINT_H_

#include <algorithm>
#include <limits>

namespace appcode {

/// Converts a double to a float, truncating finite values that are larger than
/// FLT_MAX or smaller than FLT_MIN to those values.
static float SafeNarrow(double value) {
  if (std::isinf(value) || std::isnan(value)) {
    return static_cast<float>(value);
  } else {
    // Avoid truncation to inf/-inf.
    return std::clamp(static_cast<float>(value),
                      std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::max());
  }
}

}  // namespace appcode

#endif  // APPCODE_LIB_UI_FLOATING_POINT_H_
