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

#include "appcode/shell/common/snapshot_controller.h"

#include "appcode/shell/common/snapshot_controller_skia.h"

#if IMPELLER_SUPPORTS_RENDERING
#include "appcode/shell/common/snapshot_controller_impeller.h"
#endif  // IMPELLER_SUPPORTS_RENDERING

namespace appcode {

std::unique_ptr<SnapshotController> SnapshotController::Make(
    const Delegate& delegate,
    const Settings& settings) {
#if IMPELLER_SUPPORTS_RENDERING
  if (settings.enable_impeller) {
    return std::make_unique<SnapshotControllerImpeller>(delegate);
  }
#endif  // IMPELLER_SUPPORTS_RENDERING
#if !SLIMPELLER
  return std::make_unique<SnapshotControllerSkia>(delegate);
#else   //  !SLIMPELLER
  FML_LOG(FATAL)
      << "Cannot create a Skia snapshot controller in an Impeller build.";
  return nullptr;
#endif  //  !SLIMPELLER
}

SnapshotController::SnapshotController(const Delegate& delegate)
    : delegate_(delegate) {}

}  // namespace appcode
