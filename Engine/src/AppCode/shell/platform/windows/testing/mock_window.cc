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

#include "appcode/shell/platform/windows/testing/mock_window.h"

namespace appcode {
namespace testing {
MockWindow::MockWindow() : appcodeWindow(1, 1){};
MockWindow::MockWindow(std::unique_ptr<WindowsProcTable> window_proc_table,
                       std::unique_ptr<TextInputManager> text_input_manager)
    : appcodeWindow(1,
                    1,
                    std::move(window_proc_table),
                    std::move(text_input_manager)){};

MockWindow::~MockWindow() = default;

UINT MockWindow::GetDpi() {
  return GetCurrentDPI();
}

LRESULT MockWindow::Win32DefWindowProc(HWND hWnd,
                                       UINT Msg,
                                       WPARAM wParam,
                                       LPARAM lParam) {
  return kWmResultDefault;
}

void MockWindow::SetDirectManipulationOwner(
    std::unique_ptr<DirectManipulationOwner> owner) {
  direct_manipulation_owner_ = std::move(owner);
}

LRESULT MockWindow::InjectWindowMessage(UINT const message,
                                        WPARAM const wparam,
                                        LPARAM const lparam) {
  return HandleMessage(message, wparam, lparam);
}

void MockWindow::InjectMessageList(int count, const Win32Message* messages) {
  for (int message_id = 0; message_id < count; message_id += 1) {
    const Win32Message& message = messages[message_id];
    LRESULT result =
        InjectWindowMessage(message.message, message.wParam, message.lParam);
    if (message.expected_result != kWmResultDontCheck) {
      EXPECT_EQ(result, message.expected_result);
    }
  }
}

void MockWindow::CallOnImeComposition(UINT const message,
                                      WPARAM const wparam,
                                      LPARAM const lparam) {
  appcodeWindow::OnImeComposition(message, wparam, lparam);
}

}  // namespace testing
}  // namespace appcode
