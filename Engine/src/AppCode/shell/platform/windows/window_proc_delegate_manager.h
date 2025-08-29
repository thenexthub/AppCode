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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_WINDOW_PROC_DELEGATE_MANAGER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_WINDOW_PROC_DELEGATE_MANAGER_H_

#include <Windows.h>

#include <optional>
#include <vector>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/public/appcode_windows.h"

namespace appcode {

// Handles registration, unregistration, and dispatching for WindowProc
// delegation.
class WindowProcDelegateManager {
 public:
  explicit WindowProcDelegateManager();
  ~WindowProcDelegateManager();

  // Adds |callback| as a delegate to be called for |OnTopLevelWindowProc|.
  //
  // Multiple calls with the same |callback| will replace the previous
  // registration, even if |user_data| is different.
  void RegisterTopLevelWindowProcDelegate(
      FlutterDesktopWindowProcCallback callback,
      void* user_data);

  // Unregisters |callback| as a delegate for |OnTopLevelWindowProc|.
  void UnregisterTopLevelWindowProcDelegate(
      FlutterDesktopWindowProcCallback callback);

  // Calls any registered WindowProc delegates in the order they were
  // registered.
  //
  // If a result is returned, then the message was handled in such a way that
  // further handling should not be done.
  std::optional<LRESULT> OnTopLevelWindowProc(HWND hwnd,
                                              UINT message,
                                              WPARAM wparam,
                                              LPARAM lparam) const;

 private:
  struct WindowProcDelegate {
    FlutterDesktopWindowProcCallback callback = nullptr;
    void* user_data = nullptr;
  };

  std::vector<WindowProcDelegate> delegates_;

  FML_DISALLOW_COPY_AND_ASSIGN(WindowProcDelegateManager);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_WINDOW_PROC_DELEGATE_MANAGER_H_
