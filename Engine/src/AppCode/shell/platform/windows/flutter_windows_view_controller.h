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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_WINDOWS_VIEW_CONTROLLER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_WINDOWS_VIEW_CONTROLLER_H_

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"

namespace appcode {

/// Controls a view that displays Flutter content.
class FlutterWindowsViewController {
 public:
  FlutterWindowsViewController(std::unique_ptr<FlutterWindowsEngine> engine,
                               std::unique_ptr<FlutterWindowsView> view)
      : engine_(std::move(engine)), view_(std::move(view)) {}

  ~FlutterWindowsViewController();

  // Destroy this view controller and its view.
  //
  // If this view controller owns the engine, the engine is also destroyed.
  void Destroy();

  FlutterWindowsEngine* engine() { return view_->GetEngine(); }
  FlutterWindowsView* view() { return view_.get(); }

 private:
  // The engine owned by this view controller, if any.
  //
  // This is used only if the view controller was created
  // using |FlutterDesktopViewControllerCreate| as that takes
  // ownership of the engine. Destroying this view controller
  // also destroys the engine.
  //
  // View controllers created using |FlutterDesktopEngineCreateViewController|
  // do not take ownership of the engine and this will be null. Destroying
  // this view controller does not destroy the engine.
  std::unique_ptr<FlutterWindowsEngine> engine_;

  std::unique_ptr<FlutterWindowsView> view_;

  FML_DISALLOW_COPY_AND_ASSIGN(FlutterWindowsViewController);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_WINDOWS_VIEW_CONTROLLER_H_
