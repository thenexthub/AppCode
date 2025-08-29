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

#include "appcode/shell/platform/windows/appcode_windows_view_controller.h"

namespace appcode {

appcodeWindowsViewController::~appcodeWindowsViewController() {
  Destroy();
}

void appcodeWindowsViewController::Destroy() {
  if (!view_) {
    return;
  }

  // Prevent the engine from rendering into this view.
  if (view_->GetEngine()->running()) {
    auto view_id = view_->view_id();

    view_->GetEngine()->RemoveView(view_id);
  }

  // Destroy the view, followed by the engine if it is owned by this controller.
  view_.reset();
  engine_.reset();
}

}  // namespace appcode
