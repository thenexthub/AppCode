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

#ifndef APPCODE_DISPLAY_LIST_DL_SAMPLING_OPTIONS_H_
#define APPCODE_DISPLAY_LIST_DL_SAMPLING_OPTIONS_H_

namespace appcode {

enum class DlFilterMode {
  kNearest,  // single sample point (nearest neighbor)
  kLinear,   // interporate between 2x2 sample points (bilinear interpolation)

  kLast = kLinear,
};

enum class DlImageSampling {
  kNearestNeighbor,
  kLinear,
  kMipmapLinear,
  kCubic,
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_DL_SAMPLING_OPTIONS_H_
