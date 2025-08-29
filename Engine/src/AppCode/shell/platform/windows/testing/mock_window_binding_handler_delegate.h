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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOW_BINDING_HANDLER_DELEGATE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOW_BINDING_HANDLER_DELEGATE_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/window_binding_handler_delegate.h"
#include "gmock/gmock.h"

namespace appcode {
namespace testing {

class MockWindowBindingHandlerDelegate : public WindowBindingHandlerDelegate {
 public:
  MockWindowBindingHandlerDelegate() {}

  MOCK_METHOD(bool, OnWindowSizeChanged, (size_t, size_t), (override));
  MOCK_METHOD(void, OnWindowRepaint, (), (override));
  MOCK_METHOD(void,
              OnPointerMove,
              (double, double, FlutterPointerDeviceKind, int32_t, int),
              (override));
  MOCK_METHOD(void,
              OnPointerDown,
              (double,
               double,
               FlutterPointerDeviceKind,
               int32_t,
               FlutterPointerMouseButtons),
              (override));
  MOCK_METHOD(void,
              OnPointerUp,
              (double,
               double,
               FlutterPointerDeviceKind,
               int32_t,
               FlutterPointerMouseButtons),
              (override));
  MOCK_METHOD(void,
              OnPointerLeave,
              (double, double, FlutterPointerDeviceKind, int32_t),
              (override));
  MOCK_METHOD(void, OnPointerPanZoomStart, (int32_t), (override));
  MOCK_METHOD(void,
              OnPointerPanZoomUpdate,
              (int32_t, double, double, double, double),
              (override));
  MOCK_METHOD(void, OnPointerPanZoomEnd, (int32_t), (override));
  MOCK_METHOD(void, OnText, (const std::u16string&), (override));
  MOCK_METHOD(void,
              OnKey,
              (int, int, int, char32_t, bool, bool, KeyEventCallback),
              (override));
  MOCK_METHOD(void,
              OnFocus,
              (FlutterViewFocusState, FlutterViewFocusDirection),
              (override));
  MOCK_METHOD(void, OnComposeBegin, (), (override));
  MOCK_METHOD(void, OnComposeCommit, (), (override));
  MOCK_METHOD(void, OnComposeEnd, (), (override));
  MOCK_METHOD(void, OnComposeChange, (const std::u16string&, int), (override));
  MOCK_METHOD(void, OnUpdateSemanticsEnabled, (bool), (override));
  MOCK_METHOD(gfx::NativeViewAccessible,
              GetNativeViewAccessible,
              (),
              (override));
  MOCK_METHOD(
      void,
      OnScroll,
      (double, double, double, double, int, FlutterPointerDeviceKind, int32_t),
      (override));
  MOCK_METHOD(void, OnScrollInertiaCancel, (int32_t), (override));
  MOCK_METHOD(void, OnHighContrastChanged, (), (override));

  MOCK_METHOD(ui::AXFragmentRootDelegateWin*,
              GetAxFragmentRootDelegate,
              (),
              (override));

  MOCK_METHOD(void, OnWindowStateEvent, (HWND, WindowStateEvent), (override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockWindowBindingHandlerDelegate);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOW_BINDING_HANDLER_DELEGATE_H_
