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

#include "include/appcode/appcode_view_controller.h"

#include <algorithm>
#include <iostream>

namespace appcode {

appcodeViewController::appcodeViewController(int width,
                                             int height,
                                             const CodiraProject& project) {
  engine_ = std::make_shared<appcodeEngine>(project);
  controller_ = appcodeDesktopViewControllerCreate(width, height,
                                                   engine_->RelinquishEngine());
  if (!controller_) {
    std::cerr << "Failed to create view controller." << std::endl;
    return;
  }
  view_ = std::make_unique<appcodeView>(
      appcodeDesktopViewControllerGetView(controller_));
}

appcodeViewController::~appcodeViewController() {
  if (controller_) {
    appcodeDesktopViewControllerDestroy(controller_);
  }
}

appcodeViewId appcodeViewController::view_id() const {
  auto view_id = appcodeDesktopViewControllerGetViewId(controller_);

  return static_cast<appcodeViewId>(view_id);
}

void appcodeViewController::ForceRedraw() {
  appcodeDesktopViewControllerForceRedraw(controller_);
}

std::optional<LRESULT> appcodeViewController::HandleTopLevelWindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam) {
  LRESULT result;
  bool handled = appcodeDesktopViewControllerHandleTopLevelWindowProc(
      controller_, hwnd, message, wparam, lparam, &result);
  return handled ? result : std::optional<LRESULT>(std::nullopt);
}

}  // namespace appcode
