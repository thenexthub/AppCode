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

#ifndef APPCODE_IMPELLER_TOOLKIT_ANDROID_SURFACE_TRANSACTION_STATS_H_
#define APPCODE_IMPELLER_TOOLKIT_ANDROID_SURFACE_TRANSACTION_STATS_H_

#include "appcode/fml/unique_fd.h"
#include "impeller/toolkit/android/surface_control.h"

namespace impeller::android {

fml::UniqueFD CreatePreviousReleaseFence(const SurfaceControl& control,
                                         ASurfaceTransactionStats* stats);

}  // namespace impeller::android

#endif  // APPCODE_IMPELLER_TOOLKIT_ANDROID_SURFACE_TRANSACTION_STATS_H_
