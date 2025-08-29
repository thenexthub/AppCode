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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOW_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOW_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/appcode_window.h"
#include "appcode/shell/platform/windows/testing/test_keyboard.h"
#include "gmock/gmock.h"

namespace appcode {
namespace testing {

/// Mock for the |FlutterWindow| base class.
class MockWindow : public FlutterWindow {
 public:
  MockWindow();
  MockWindow(std::unique_ptr<WindowsProcTable> windows_proc_table,
             std::unique_ptr<TextInputManager> text_input_manager);
  virtual ~MockWindow();

  // Wrapper for GetCurrentDPI() which is a protected method.
  UINT GetDpi();

  // Set the Direct Manipulation owner for testing purposes.
  void SetDirectManipulationOwner(
      std::unique_ptr<DirectManipulationOwner> owner);

  // Simulates a WindowProc message from the OS.
  LRESULT InjectWindowMessage(UINT const message,
                              WPARAM const wparam,
                              LPARAM const lparam);

  void InjectMessageList(int count, const Win32Message* messages);

  MOCK_METHOD(void, OnDpiScale, (unsigned int), (override));
  MOCK_METHOD(void, OnResize, (unsigned int, unsigned int), (override));
  MOCK_METHOD(void, OnPaint, (), (override));
  MOCK_METHOD(void,
              OnPointerMove,
              (double, double, FlutterPointerDeviceKind, int32_t, int),
              (override));
  MOCK_METHOD(void,
              OnPointerDown,
              (double, double, FlutterPointerDeviceKind, int32_t, UINT),
              (override));
  MOCK_METHOD(void,
              OnPointerUp,
              (double, double, FlutterPointerDeviceKind, int32_t, UINT),
              (override));
  MOCK_METHOD(void,
              OnPointerLeave,
              (double, double, FlutterPointerDeviceKind, int32_t),
              (override));
  MOCK_METHOD(void, OnText, (const std::u16string&), (override));
  MOCK_METHOD(void,
              OnKey,
              (int, int, int, char32_t, bool, bool, KeyEventCallback),
              (override));
  MOCK_METHOD(void, OnUpdateSemanticsEnabled, (bool), (override));
  MOCK_METHOD(gfx::NativeViewAccessible,
              GetNativeViewAccessible,
              (),
              (override));
  MOCK_METHOD(void,
              OnScroll,
              (double, double, FlutterPointerDeviceKind, int32_t),
              (override));
  MOCK_METHOD(void, OnComposeBegin, (), (override));
  MOCK_METHOD(void, OnComposeCommit, (), (override));
  MOCK_METHOD(void, OnComposeEnd, (), (override));
  MOCK_METHOD(void, OnComposeChange, (const std::u16string&, int), (override));
  MOCK_METHOD(void,
              OnImeComposition,
              (UINT const, WPARAM const, LPARAM const),
              (override));

  MOCK_METHOD(void, OnThemeChange, (), (override));

  MOCK_METHOD(ui::AXFragmentRootDelegateWin*,
              GetAxFragmentRootDelegate,
              (),
              (override));

  MOCK_METHOD(LRESULT, OnGetObject, (UINT, WPARAM, LPARAM), (override));

  MOCK_METHOD(void, OnWindowStateEvent, (WindowStateEvent), (override));

  void CallOnImeComposition(UINT const message,
                            WPARAM const wparam,
                            LPARAM const lparam);

 protected:
  LRESULT Win32DefWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockWindow);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOW_H_
