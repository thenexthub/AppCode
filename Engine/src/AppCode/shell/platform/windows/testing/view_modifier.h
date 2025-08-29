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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_VIEW_MODIFIER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_VIEW_MODIFIER_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/egl/window_surface.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"

namespace appcode {

// A test utility class providing the ability to access and alter various
// private fields in a |FlutterWindowsView| instance.
class ViewModifier {
 public:
  explicit ViewModifier(FlutterWindowsView* view) : view_(view) {}

  // Override the EGL surface used by the view.
  //
  // Modifications are to the view, and will last for the lifetime of the
  // view unless overwritten again.
  void SetSurface(std::unique_ptr<egl::WindowSurface> surface) {
    view_->surface_ = std::move(surface);
  }

 private:
  FlutterWindowsView* view_;

  FML_DISALLOW_COPY_AND_ASSIGN(ViewModifier);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_VIEW_MODIFIER_H_
