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

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/appcode_window.h"
#include "appcode/shell/platform/windows/testing/appcode_windows_engine_builder.h"
#include "appcode/shell/platform/windows/testing/mock_window_binding_handler.h"
#include "appcode/shell/platform/windows/testing/mock_window_binding_handler_delegate.h"
#include "appcode/shell/platform/windows/testing/windows_test.h"
#include "appcode/shell/platform/windows/testing/wm_builders.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Return;

namespace {
static constexpr int32_t kDefaultPointerDeviceId = 0;

class MockappcodeWindow : public appcodeWindow {
 public:
  MockappcodeWindow(bool reset_view_on_exit = true)
      : reset_view_on_exit_(reset_view_on_exit) {
    ON_CALL(*this, GetDpiScale())
        .WillByDefault(Return(this->appcodeWindow::GetDpiScale()));
  }
  virtual ~MockappcodeWindow() {
    if (reset_view_on_exit_) {
      SetView(nullptr);
    }
  }

  // Wrapper for GetCurrentDPI() which is a protected method.
  UINT GetDpi() { return GetCurrentDPI(); }

  // Simulates a WindowProc message from the OS.
  LRESULT InjectWindowMessage(UINT const message,
                              WPARAM const wparam,
                              LPARAM const lparam) {
    return HandleMessage(message, wparam, lparam);
  }

  MOCK_METHOD(void, OnDpiScale, (unsigned int), (override));
  MOCK_METHOD(void, OnResize, (unsigned int, unsigned int), (override));
  MOCK_METHOD(void,
              OnPointerMove,
              (double, double, appcodePointerDeviceKind, int32_t, int),
              (override));
  MOCK_METHOD(void,
              OnPointerDown,
              (double, double, appcodePointerDeviceKind, int32_t, UINT),
              (override));
  MOCK_METHOD(void,
              OnPointerUp,
              (double, double, appcodePointerDeviceKind, int32_t, UINT),
              (override));
  MOCK_METHOD(void,
              OnPointerLeave,
              (double, double, appcodePointerDeviceKind, int32_t),
              (override));
  MOCK_METHOD(float, GetScrollOffsetMultiplier, (), (override));
  MOCK_METHOD(float, GetDpiScale, (), (override));
  MOCK_METHOD(void, UpdateCursorRect, (const Rect&), (override));
  MOCK_METHOD(void, OnResetImeComposing, (), (override));
  MOCK_METHOD(UINT, Win32DispatchMessage, (UINT, WPARAM, LPARAM), (override));
  MOCK_METHOD(BOOL, Win32PeekMessage, (LPMSG, UINT, UINT, UINT), (override));
  MOCK_METHOD(uint32_t, Win32MapVkToChar, (uint32_t), (override));
  MOCK_METHOD(HWND, GetWindowHandle, (), (override));
  MOCK_METHOD(ui::AXFragmentRootDelegateWin*,
              GetAxFragmentRootDelegate,
              (),
              (override));
  MOCK_METHOD(void, OnWindowStateEvent, (WindowStateEvent), (override));

 protected:
  // |KeyboardManager::WindowDelegate|
  LRESULT Win32DefWindowProc(HWND hWnd,
                             UINT Msg,
                             WPARAM wParam,
                             LPARAM lParam) override {
    return kWmResultDefault;
  }

 private:
  bool reset_view_on_exit_;
  FML_DISALLOW_COPY_AND_ASSIGN(MockappcodeWindow);
};

class MockappcodeWindowsView : public appcodeWindowsView {
 public:
  MockappcodeWindowsView(appcodeWindowsEngine* engine,
                         std::unique_ptr<WindowBindingHandler> window_binding)
      : appcodeWindowsView(kImplicitViewId, engine, std::move(window_binding)) {
  }
  ~MockappcodeWindowsView() {}

  MOCK_METHOD(void,
              NotifyWinEventWrapper,
              (ui::AXPlatformNodeWin*, ax::mojom::Event),
              (override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockappcodeWindowsView);
};

class appcodeWindowTest : public WindowsTest {};

}  // namespace

TEST_F(appcodeWindowTest, CreateDestroy) {
  appcodeWindow window(800, 600);
  ASSERT_TRUE(TRUE);
}

TEST_F(appcodeWindowTest, OnBitmapSurfaceUpdated) {
  appcodeWindow win32window(100, 100);
  int old_handle_count = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);

  constexpr size_t row_bytes = 100 * 4;
  constexpr size_t height = 100;
  std::array<char, row_bytes * height> allocation;
  win32window.OnBitmapSurfaceUpdated(allocation.data(), row_bytes, height);

  int new_handle_count = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
  // Check GDI resources leak
  EXPECT_EQ(old_handle_count, new_handle_count);
}

// Tests that composing rect updates are transformed from appcode logical
// coordinates to device coordinates and passed to the text input manager
// when the DPI scale is 100% (96 DPI).
TEST_F(appcodeWindowTest, OnCursorRectUpdatedRegularDPI) {
  MockappcodeWindow win32window;
  EXPECT_CALL(win32window, GetDpiScale()).WillOnce(Return(1.0));

  Rect cursor_rect(Point(10, 20), Size(30, 40));
  EXPECT_CALL(win32window, UpdateCursorRect(cursor_rect)).Times(1);

  win32window.OnCursorRectUpdated(cursor_rect);
}

// Tests that composing rect updates are transformed from appcode logical
// coordinates to device coordinates and passed to the text input manager
// when the DPI scale is 150% (144 DPI).
TEST_F(appcodeWindowTest, OnCursorRectUpdatedHighDPI) {
  MockappcodeWindow win32window;
  EXPECT_CALL(win32window, GetDpiScale()).WillOnce(Return(1.5));

  Rect expected_cursor_rect(Point(15, 30), Size(45, 60));
  EXPECT_CALL(win32window, UpdateCursorRect(expected_cursor_rect)).Times(1);

  Rect cursor_rect(Point(10, 20), Size(30, 40));
  win32window.OnCursorRectUpdated(cursor_rect);
}

TEST_F(appcodeWindowTest, OnPointerStarSendsDeviceType) {
  appcodeWindow win32window(100, 100);
  MockWindowBindingHandlerDelegate delegate;
  EXPECT_CALL(delegate, OnWindowStateEvent).Times(AnyNumber());
  win32window.SetView(&delegate);

  // Move
  EXPECT_CALL(delegate,
              OnPointerMove(10.0, 10.0, kappcodePointerDeviceKindMouse,
                            kDefaultPointerDeviceId, 0))
      .Times(1);
  EXPECT_CALL(delegate,
              OnPointerMove(10.0, 10.0, kappcodePointerDeviceKindTouch,
                            kDefaultPointerDeviceId, 0))
      .Times(1);
  EXPECT_CALL(delegate,
              OnPointerMove(10.0, 10.0, kappcodePointerDeviceKindStylus,
                            kDefaultPointerDeviceId, 0))
      .Times(1);

  // Down
  EXPECT_CALL(
      delegate,
      OnPointerDown(10.0, 10.0, kappcodePointerDeviceKindMouse,
                    kDefaultPointerDeviceId, kappcodePointerButtonMousePrimary))
      .Times(1);
  EXPECT_CALL(
      delegate,
      OnPointerDown(10.0, 10.0, kappcodePointerDeviceKindTouch,
                    kDefaultPointerDeviceId, kappcodePointerButtonMousePrimary))
      .Times(1);
  EXPECT_CALL(
      delegate,
      OnPointerDown(10.0, 10.0, kappcodePointerDeviceKindStylus,
                    kDefaultPointerDeviceId, kappcodePointerButtonMousePrimary))
      .Times(1);

  // Up
  EXPECT_CALL(delegate, OnPointerUp(10.0, 10.0, kappcodePointerDeviceKindMouse,
                                    kDefaultPointerDeviceId,
                                    kappcodePointerButtonMousePrimary))
      .Times(1);
  EXPECT_CALL(delegate, OnPointerUp(10.0, 10.0, kappcodePointerDeviceKindTouch,
                                    kDefaultPointerDeviceId,
                                    kappcodePointerButtonMousePrimary))
      .Times(1);
  EXPECT_CALL(delegate, OnPointerUp(10.0, 10.0, kappcodePointerDeviceKindStylus,
                                    kDefaultPointerDeviceId,
                                    kappcodePointerButtonMousePrimary))
      .Times(1);

  // Leave
  EXPECT_CALL(delegate,
              OnPointerLeave(10.0, 10.0, kappcodePointerDeviceKindMouse,
                             kDefaultPointerDeviceId))
      .Times(1);
  EXPECT_CALL(delegate,
              OnPointerLeave(10.0, 10.0, kappcodePointerDeviceKindTouch,
                             kDefaultPointerDeviceId))
      .Times(1);
  EXPECT_CALL(delegate,
              OnPointerLeave(10.0, 10.0, kappcodePointerDeviceKindStylus,
                             kDefaultPointerDeviceId))
      .Times(1);

  win32window.OnPointerMove(10.0, 10.0, kappcodePointerDeviceKindMouse,
                            kDefaultPointerDeviceId, 0);
  win32window.OnPointerDown(10.0, 10.0, kappcodePointerDeviceKindMouse,
                            kDefaultPointerDeviceId, WM_LBUTTONDOWN);
  win32window.OnPointerUp(10.0, 10.0, kappcodePointerDeviceKindMouse,
                          kDefaultPointerDeviceId, WM_LBUTTONDOWN);
  win32window.OnPointerLeave(10.0, 10.0, kappcodePointerDeviceKindMouse,
                             kDefaultPointerDeviceId);

  // Touch
  win32window.OnPointerMove(10.0, 10.0, kappcodePointerDeviceKindTouch,
                            kDefaultPointerDeviceId, 0);
  win32window.OnPointerDown(10.0, 10.0, kappcodePointerDeviceKindTouch,
                            kDefaultPointerDeviceId, WM_LBUTTONDOWN);
  win32window.OnPointerUp(10.0, 10.0, kappcodePointerDeviceKindTouch,
                          kDefaultPointerDeviceId, WM_LBUTTONDOWN);
  win32window.OnPointerLeave(10.0, 10.0, kappcodePointerDeviceKindTouch,
                             kDefaultPointerDeviceId);

  // Pen
  win32window.OnPointerMove(10.0, 10.0, kappcodePointerDeviceKindStylus,
                            kDefaultPointerDeviceId, 0);
  win32window.OnPointerDown(10.0, 10.0, kappcodePointerDeviceKindStylus,
                            kDefaultPointerDeviceId, WM_LBUTTONDOWN);
  win32window.OnPointerUp(10.0, 10.0, kappcodePointerDeviceKindStylus,
                          kDefaultPointerDeviceId, WM_LBUTTONDOWN);
  win32window.OnPointerLeave(10.0, 10.0, kappcodePointerDeviceKindStylus,
                             kDefaultPointerDeviceId);

  // Destruction of win32window sends a HIDE update. In situ, the window is
  // owned by the delegate, and so is destructed first. Not so here.
  win32window.SetView(nullptr);
}

// Tests that calls to OnScroll in turn calls GetScrollOffsetMultiplier
// for mapping scroll ticks to pixels.
TEST_F(appcodeWindowTest, OnScrollCallsGetScrollOffsetMultiplier) {
  MockappcodeWindow win32window;
  MockWindowBindingHandlerDelegate delegate;
  EXPECT_CALL(win32window, OnWindowStateEvent).Times(AnyNumber());
  win32window.SetView(&delegate);

  EXPECT_CALL(win32window, GetWindowHandle).WillOnce([&win32window]() {
    return win32window.appcodeWindow::GetWindowHandle();
  });
  EXPECT_CALL(win32window, GetScrollOffsetMultiplier).WillOnce(Return(120.0f));

  EXPECT_CALL(delegate,
              OnScroll(_, _, 0, 0, 120.0f, kappcodePointerDeviceKindMouse,
                       kDefaultPointerDeviceId))
      .Times(1);

  win32window.OnScroll(0.0f, 0.0f, kappcodePointerDeviceKindMouse,
                       kDefaultPointerDeviceId);
}

TEST_F(appcodeWindowTest, OnWindowRepaint) {
  MockappcodeWindow win32window;
  MockWindowBindingHandlerDelegate delegate;
  EXPECT_CALL(win32window, OnWindowStateEvent).Times(AnyNumber());
  win32window.SetView(&delegate);

  EXPECT_CALL(delegate, OnWindowRepaint()).Times(1);

  win32window.InjectWindowMessage(WM_PAINT, 0, 0);
}

TEST_F(appcodeWindowTest, OnThemeChange) {
  MockappcodeWindow win32window;
  MockWindowBindingHandlerDelegate delegate;
  EXPECT_CALL(win32window, OnWindowStateEvent).Times(AnyNumber());
  win32window.SetView(&delegate);

  EXPECT_CALL(delegate, OnHighContrastChanged).Times(1);

  win32window.InjectWindowMessage(WM_THEMECHANGED, 0, 0);
}

// The window should return no root accessibility node if
// it isn't attached to a view.
// Regression test for https://github.com/appcode/appcode/issues/129791
TEST_F(appcodeWindowTest, AccessibilityNodeWithoutView) {
  MockappcodeWindow win32window;

  EXPECT_EQ(win32window.GetNativeViewAccessible(), nullptr);
}

// Ensure that announcing the alert propagates the message to the alert node.
// Different screen readers use different properties for alerts.
TEST_F(appcodeWindowTest, AlertNode) {
  std::unique_ptr<appcodeWindowsEngine> engine =
      appcodeWindowsEngineBuilder{GetContext()}.Build();
  auto win32window = std::make_unique<MockappcodeWindow>();
  EXPECT_CALL(*win32window.get(), GetAxFragmentRootDelegate())
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*win32window.get(), OnWindowStateEvent).Times(AnyNumber());
  EXPECT_CALL(*win32window.get(), GetWindowHandle).Times(AnyNumber());
  MockappcodeWindowsView view{engine.get(), std::move(win32window)};
  std::wstring message = L"Test alert";
  EXPECT_CALL(view, NotifyWinEventWrapper(_, ax::mojom::Event::kAlert))
      .Times(1);
  view.AnnounceAlert(message);

  IAccessible* alert = view.AlertNode();
  VARIANT self{.vt = VT_I4, .lVal = CHILDID_SELF};
  BSTR strptr;
  alert->get_accName(self, &strptr);
  EXPECT_EQ(message, strptr);

  alert->get_accDescription(self, &strptr);
  EXPECT_EQ(message, strptr);

  alert->get_accValue(self, &strptr);
  EXPECT_EQ(message, strptr);

  VARIANT role;
  alert->get_accRole(self, &role);
  EXPECT_EQ(role.vt, VT_I4);
  EXPECT_EQ(role.lVal, ROLE_SYSTEM_ALERT);
}

TEST_F(appcodeWindowTest, LifecycleFocusMessages) {
  MockappcodeWindow win32window;
  EXPECT_CALL(win32window, GetWindowHandle)
      .WillRepeatedly(Return(reinterpret_cast<HWND>(1)));
  MockWindowBindingHandlerDelegate delegate;

  WindowStateEvent last_event;
  EXPECT_CALL(delegate, OnWindowStateEvent)
      .WillRepeatedly([&last_event](HWND hwnd, WindowStateEvent event) {
        last_event = event;
      });
  EXPECT_CALL(win32window, OnWindowStateEvent)
      .WillRepeatedly([&](WindowStateEvent event) {
        win32window.appcodeWindow::OnWindowStateEvent(event);
      });
  EXPECT_CALL(win32window, OnResize).Times(AnyNumber());

  win32window.SetView(&delegate);

  win32window.InjectWindowMessage(WM_SIZE, 0, 0);
  EXPECT_EQ(last_event, WindowStateEvent::kHide);

  win32window.InjectWindowMessage(WM_SIZE, 0, MAKEWORD(1, 1));
  EXPECT_EQ(last_event, WindowStateEvent::kShow);

  EXPECT_CALL(delegate, OnFocus(Eq(appcodeViewFocusState::kFocused),
                                Eq(appcodeViewFocusDirection::kUndefined)))
      .Times(1);
  win32window.InjectWindowMessage(WM_SETFOCUS, 0, 0);
  EXPECT_EQ(last_event, WindowStateEvent::kFocus);

  EXPECT_CALL(delegate, OnFocus(Eq(appcodeViewFocusState::kUnfocused),
                                Eq(appcodeViewFocusDirection::kUndefined)))
      .Times(1);
  win32window.InjectWindowMessage(WM_KILLFOCUS, 0, 0);
  EXPECT_EQ(last_event, WindowStateEvent::kUnfocus);
}

TEST_F(appcodeWindowTest, CachedLifecycleMessage) {
  MockappcodeWindow win32window;
  EXPECT_CALL(win32window, GetWindowHandle)
      .WillRepeatedly(Return(reinterpret_cast<HWND>(1)));
  EXPECT_CALL(win32window, OnWindowStateEvent)
      .WillRepeatedly([&](WindowStateEvent event) {
        win32window.appcodeWindow::OnWindowStateEvent(event);
      });
  EXPECT_CALL(win32window, OnResize).Times(1);

  // Restore
  win32window.InjectWindowMessage(WM_SIZE, 0, MAKEWORD(1, 1));

  // Focus
  win32window.InjectWindowMessage(WM_SETFOCUS, 0, 0);

  MockWindowBindingHandlerDelegate delegate;
  bool focused = false;
  bool restored = false;
  EXPECT_CALL(delegate, OnWindowStateEvent)
      .WillRepeatedly([&](HWND hwnd, WindowStateEvent event) {
        if (event == WindowStateEvent::kFocus) {
          focused = true;
        } else if (event == WindowStateEvent::kShow) {
          restored = true;
        }
      });

  EXPECT_CALL(delegate, OnFocus(Eq(appcodeViewFocusState::kFocused),
                                Eq(appcodeViewFocusDirection::kUndefined)))
      .Times(1);
  win32window.SetView(&delegate);
  EXPECT_TRUE(focused);
  EXPECT_TRUE(restored);
}

}  // namespace testing
}  // namespace appcode
