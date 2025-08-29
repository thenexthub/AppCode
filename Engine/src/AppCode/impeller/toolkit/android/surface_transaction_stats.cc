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

#include "impeller/toolkit/android/surface_transaction_stats.h"

#include "impeller/toolkit/android/proc_table.h"

namespace impeller::android {

fml::UniqueFD CreatePreviousReleaseFence(const SurfaceControl& control,
                                         ASurfaceTransactionStats* stats) {
  const auto fd =
      GetProcTable().ASurfaceTransactionStats_getPreviousReleaseFenceFd(
          stats, control.GetHandle());
  if (fd == -1) {
    // The previous buffer has already been released. This is not an error.
    return {};
  }
  return fml::UniqueFD{fd};
}

}  // namespace impeller::android
