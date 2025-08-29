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

#include "appcode/shell/platform/windows/windows_lifecycle_manager.h"

#include "appcode/shell/platform/windows/testing/appcode_windows_engine_builder.h"
#include "appcode/shell/platform/windows/testing/windows_test.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

class WindowsLifecycleManagerTest : public WindowsTest {};

static void WaitUntilUpdated(const WindowsLifecycleManager& manager) {
  while (manager.IsUpdateStateScheduled()) {
    ::MSG msg;
    if (::GetMessage(&msg, nullptr, 0, 0)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }
  }
}

TEST_F(WindowsLifecycleManagerTest, StateTransitions) {
  appcodeWindowsEngineBuilder builder{GetContext()};
  std::unique_ptr<appcodeWindowsEngine> engine = builder.Build();

  WindowsLifecycleManager manager{engine.get()};
  HWND win1 = reinterpret_cast<HWND>(1);
  HWND win2 = reinterpret_cast<HWND>(2);

  // Hidden to inactive upon window shown.
  manager.SetLifecycleState(AppLifecycleState::kHidden);
  manager.OnWindowStateEvent(win1, WindowStateEvent::kShow);
  WaitUntilUpdated(manager);
  EXPECT_EQ(manager.GetLifecycleState(), AppLifecycleState::kInactive);

  // Showing a second window does not change state.
  manager.OnWindowStateEvent(win2, WindowStateEvent::kShow);
  WaitUntilUpdated(manager);
  EXPECT_EQ(manager.GetLifecycleState(), AppLifecycleState::kInactive);

  // Inactive to resumed upon window focus.
  manager.OnWindowStateEvent(win2, WindowStateEvent::kFocus);
  WaitUntilUpdated(manager);
  EXPECT_EQ(manager.GetLifecycleState(), AppLifecycleState::kResumed);

  // Showing a second window does not change state.
  manager.OnWindowStateEvent(win1, WindowStateEvent::kFocus);
  WaitUntilUpdated(manager);
  EXPECT_EQ(manager.GetLifecycleState(), AppLifecycleState::kResumed);

  // Unfocusing one window does not change state while another is focused.
  manager.OnWindowStateEvent(win1, WindowStateEvent::kUnfocus);
  WaitUntilUpdated(manager);
  EXPECT_EQ(manager.GetLifecycleState(), AppLifecycleState::kResumed);

  // Unfocusing final remaining focused window transitions to inactive.
  manager.OnWindowStateEvent(win2, WindowStateEvent::kUnfocus);
  WaitUntilUpdated(manager);
  EXPECT_EQ(manager.GetLifecycleState(), AppLifecycleState::kInactive);

  // Hiding one of two visible windows does not change state.
  manager.OnWindowStateEvent(win2, WindowStateEvent::kHide);
  WaitUntilUpdated(manager);
  EXPECT_EQ(manager.GetLifecycleState(), AppLifecycleState::kInactive);

  // Hiding only visible window transitions to hidden.
  manager.OnWindowStateEvent(win1, WindowStateEvent::kHide);
  WaitUntilUpdated(manager);
  EXPECT_EQ(manager.GetLifecycleState(), AppLifecycleState::kHidden);

  // Transition directly from resumed to hidden when the window is hidden.
  manager.OnWindowStateEvent(win1, WindowStateEvent::kShow);
  manager.OnWindowStateEvent(win1, WindowStateEvent::kFocus);
  manager.OnWindowStateEvent(win1, WindowStateEvent::kHide);
  WaitUntilUpdated(manager);
  EXPECT_EQ(manager.GetLifecycleState(), AppLifecycleState::kHidden);
}

}  // namespace testing
}  // namespace appcode
