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

#include "check_view.h"

#include "appcode/fml/logging.h"

namespace fuchsia_test_utils {

bool CheckViewExistsInSnapshot(
    const fuchsia::ui::observation::geometry::ViewTreeSnapshot& snapshot,
    zx_koid_t view_ref_koid) {
  if (!snapshot.has_views()) {
    return false;
  }

  auto snapshot_count =
      std::count_if(snapshot.views().begin(), snapshot.views().end(),
                    [view_ref_koid](const auto& view) {
                      return view.view_ref_koid() == view_ref_koid;
                    });

  return snapshot_count > 0;
}

bool CheckViewExistsInUpdates(
    const std::vector<fuchsia::ui::observation::geometry::ViewTreeSnapshot>&
        updates,
    zx_koid_t view_ref_koid) {
  auto update_count = std::count_if(
      updates.begin(), updates.end(), [view_ref_koid](auto& snapshot) {
        return CheckViewExistsInSnapshot(snapshot, view_ref_koid);
      });

  return update_count > 0;
}

}  // namespace fuchsia_test_utils
