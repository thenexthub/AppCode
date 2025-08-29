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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_INTEGRATION_UTILS_CHECK_VIEW_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_INTEGRATION_UTILS_CHECK_VIEW_H_

#include <vector>

#include <fuchsia/ui/observation/geometry/cpp/fidl.h>
#include <zircon/status.h>

namespace fuchsia_test_utils {

/// Returns true if a view with the given |view_ref_koid| exists in a |snapshot|
/// of the view tree, false otherwise.
bool CheckViewExistsInSnapshot(
    const fuchsia::ui::observation::geometry::ViewTreeSnapshot& snapshot,
    zx_koid_t view_ref_koid);

/// Returns true if any of the snapshots of the view tree in |updates| contain a
/// view with the given |view_ref_koid|, false otherwise.
bool CheckViewExistsInUpdates(
    const std::vector<fuchsia::ui::observation::geometry::ViewTreeSnapshot>&
        updates,
    zx_koid_t view_ref_koid);

}  // namespace fuchsia_test_utils

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_INTEGRATION_UTILS_CHECK_VIEW_H_
