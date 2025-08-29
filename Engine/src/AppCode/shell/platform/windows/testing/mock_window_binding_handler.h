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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOW_BINDING_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOW_BINDING_HANDLER_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/window_binding_handler.h"
#include "appcode/third_party/accessibility/ax/platform/ax_platform_node_win.h"
#include "gmock/gmock.h"

namespace appcode {
namespace testing {

/// Mock for the |Window| base class.
class MockWindowBindingHandler : public WindowBindingHandler {
 public:
  MockWindowBindingHandler();
  virtual ~MockWindowBindingHandler();

  MOCK_METHOD(void, SetView, (WindowBindingHandlerDelegate * view), (override));
  MOCK_METHOD(HWND, GetWindowHandle, (), (override));
  MOCK_METHOD(float, GetDpiScale, (), (override));
  MOCK_METHOD(PhysicalWindowBounds, GetPhysicalWindowBounds, (), (override));
  MOCK_METHOD(void, OnCursorRectUpdated, (const Rect& rect), (override));
  MOCK_METHOD(void, OnResetImeComposing, (), (override));
  MOCK_METHOD(bool, OnBitmapSurfaceCleared, (), (override));
  MOCK_METHOD(bool,
              OnBitmapSurfaceUpdated,
              (const void* allocation, size_t row_bytes, size_t height),
              (override));
  MOCK_METHOD(PointerLocation, GetPrimaryPointerLocation, (), (override));
  MOCK_METHOD(AlertPlatformNodeDelegate*, GetAlertDelegate, (), (override));
  MOCK_METHOD(ui::AXPlatformNodeWin*, GetAlert, (), (override));
  MOCK_METHOD(bool, Focus, (), (override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockWindowBindingHandler);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOW_BINDING_HANDLER_H_
