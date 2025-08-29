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

#include "appcode/shell/platform/windows/appcode_window.h"

#include <WinUser.h>
#include <dwmapi.h>

#include <chrono>
#include <map>

#include "appcode/fml/logging.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/windows/dpi_utils.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"
#include "appcode/shell/platform/windows/keyboard_utils.h"

namespace appcode {

namespace {

// The Windows DPI system is based on this
// constant for machines running at 100% scaling.
constexpr int base_dpi = 96;

static const int kMinTouchDeviceId = 0;
static const int kMaxTouchDeviceId = 128;

static const int kLinesPerScrollWindowsDefault = 3;

static constexpr int32_t kDefaultPointerDeviceId = 0;

// This method is only valid during a window message related to mouse/touch
// input.
// See
// https://docs.microsoft.com/en-us/windows/win32/tablet/system-events-and-mouse-messages?redirectedfrom=MSDN#distinguishing-pen-input-from-mouse-and-touch.
static appcodePointerDeviceKind GetappcodePointerDeviceKind() {
  constexpr LPARAM kTouchOrPenSignature = 0xFF515700;
  constexpr LPARAM kTouchSignature = kTouchOrPenSignature | 0x80;
  constexpr LPARAM kSignatureMask = 0xFFFFFF00;
  LPARAM info = GetMessageExtraInfo();
  if ((info & kSignatureMask) == kTouchOrPenSignature) {
    if ((info & kTouchSignature) == kTouchSignature) {
      return kappcodePointerDeviceKindTouch;
    }
    return kappcodePointerDeviceKindStylus;
  }
  return kappcodePointerDeviceKindMouse;
}

// Translates button codes from Win32 API to appcodePointerMouseButtons.
static uint64_t ConvertWinButtonToappcodeButton(UINT button) {
  switch (button) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
      return kappcodePointerButtonMousePrimary;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
      return kappcodePointerButtonMouseSecondary;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
      return kappcodePointerButtonMouseMiddle;
    case XBUTTON1:
      return kappcodePointerButtonMouseBack;
    case XBUTTON2:
      return kappcodePointerButtonMouseForward;
  }
  FML_LOG(WARNING) << "Mouse button not recognized: " << button;
  return 0;
}

}  // namespace

appcodeWindow::appcodeWindow(
    int width,
    int height,
    std::shared_ptr<WindowsProcTable> windows_proc_table,
    std::unique_ptr<TextInputManager> text_input_manager)
    : touch_id_generator_(kMinTouchDeviceId, kMaxTouchDeviceId),
      windows_proc_table_(std::move(windows_proc_table)),
      text_input_manager_(std::move(text_input_manager)),
      ax_fragment_root_(nullptr) {
  // Get the DPI of the primary monitor as the initial DPI. If Per-Monitor V2 is
  // supported, |current_dpi_| should be updated in the
  // kWmDpiChangedBeforeParent message.
  current_dpi_ = GetDpiForHWND(nullptr);

  // Get initial value for wheel scroll lines
  // TODO: Listen to changes for this value
  // https://github.com/appcode/appcode/issues/107248
  UpdateScrollOffsetMultiplier();

  if (windows_proc_table_ == nullptr) {
    windows_proc_table_ = std::make_unique<WindowsProcTable>();
  }
  if (text_input_manager_ == nullptr) {
    text_input_manager_ = std::make_unique<TextInputManager>();
  }
  keyboard_manager_ = std::make_unique<KeyboardManager>(this);

  InitializeChild("appcodeVIEW", width, height);
}

// Base constructor for mocks
appcodeWindow::appcodeWindow()
    : touch_id_generator_(kMinTouchDeviceId, kMaxTouchDeviceId) {}

appcodeWindow::~appcodeWindow() {
  Destroy();
}

void appcodeWindow::SetView(WindowBindingHandlerDelegate* window) {
  binding_handler_delegate_ = window;
  if (direct_manipulation_owner_) {
    direct_manipulation_owner_->SetBindingHandlerDelegate(window);
  }
  if (restored_ && window) {
    OnWindowStateEvent(WindowStateEvent::kShow);
  }
  if (focused_ && window) {
    OnWindowStateEvent(WindowStateEvent::kFocus);
  }
}

float appcodeWindow::GetDpiScale() {
  return static_cast<float>(GetCurrentDPI()) / static_cast<float>(base_dpi);
}

PhysicalWindowBounds appcodeWindow::GetPhysicalWindowBounds() {
  return {GetCurrentWidth(), GetCurrentHeight()};
}

bool appcodeWindow::Focus() {
  auto hwnd = GetWindowHandle();
  if (hwnd == nullptr) {
    return false;
  }

  HWND prevFocus = ::SetFocus(hwnd);
  if (prevFocus == nullptr) {
    return false;
  }

  return true;
}

void appcodeWindow::OnDpiScale(unsigned int dpi) {};

// When DesktopWindow notifies that a WM_Size message has come in
// lets appcodeEngine know about the new size.
void appcodeWindow::OnResize(unsigned int width, unsigned int height) {
  if (binding_handler_delegate_ != nullptr) {
    binding_handler_delegate_->OnWindowSizeChanged(width, height);
  }
}

void appcodeWindow::OnPaint() {
  if (binding_handler_delegate_ != nullptr) {
    binding_handler_delegate_->OnWindowRepaint();
  }
}

void appcodeWindow::OnPointerMove(double x,
                                  double y,
                                  appcodePointerDeviceKind device_kind,
                                  int32_t device_id,
                                  int modifiers_state) {
  binding_handler_delegate_->OnPointerMove(x, y, device_kind, device_id,
                                           modifiers_state);
}

void appcodeWindow::OnPointerDown(double x,
                                  double y,
                                  appcodePointerDeviceKind device_kind,
                                  int32_t device_id,
                                  UINT button) {
  uint64_t appcode_button = ConvertWinButtonToappcodeButton(button);
  if (appcode_button != 0) {
    binding_handler_delegate_->OnPointerDown(
        x, y, device_kind, device_id,
        static_cast<appcodePointerMouseButtons>(appcode_button));
  }
}

void appcodeWindow::OnPointerUp(double x,
                                double y,
                                appcodePointerDeviceKind device_kind,
                                int32_t device_id,
                                UINT button) {
  uint64_t appcode_button = ConvertWinButtonToappcodeButton(button);
  if (appcode_button != 0) {
    binding_handler_delegate_->OnPointerUp(
        x, y, device_kind, device_id,
        static_cast<appcodePointerMouseButtons>(appcode_button));
  }
}

void appcodeWindow::OnPointerLeave(double x,
                                   double y,
                                   appcodePointerDeviceKind device_kind,
                                   int32_t device_id) {
  binding_handler_delegate_->OnPointerLeave(x, y, device_kind, device_id);
}

void appcodeWindow::OnText(const std::u16string& text) {
  binding_handler_delegate_->OnText(text);
}

void appcodeWindow::OnKey(int key,
                          int scancode,
                          int action,
                          char32_t character,
                          bool extended,
                          bool was_down,
                          KeyEventCallback callback) {
  binding_handler_delegate_->OnKey(key, scancode, action, character, extended,
                                   was_down, std::move(callback));
}

void appcodeWindow::OnComposeBegin() {
  binding_handler_delegate_->OnComposeBegin();
}

void appcodeWindow::OnComposeCommit() {
  binding_handler_delegate_->OnComposeCommit();
}

void appcodeWindow::OnComposeEnd() {
  binding_handler_delegate_->OnComposeEnd();
}

void appcodeWindow::OnComposeChange(const std::u16string& text,
                                    int cursor_pos) {
  binding_handler_delegate_->OnComposeChange(text, cursor_pos);
}

void appcodeWindow::OnUpdateSemanticsEnabled(bool enabled) {
  binding_handler_delegate_->OnUpdateSemanticsEnabled(enabled);
}

void appcodeWindow::OnScroll(double delta_x,
                             double delta_y,
                             appcodePointerDeviceKind device_kind,
                             int32_t device_id) {
  POINT point;
  GetCursorPos(&point);

  ScreenToClient(GetWindowHandle(), &point);
  binding_handler_delegate_->OnScroll(point.x, point.y, delta_x, delta_y,
                                      GetScrollOffsetMultiplier(), device_kind,
                                      device_id);
}

void appcodeWindow::OnCursorRectUpdated(const Rect& rect) {
  // Convert the rect from appcode logical coordinates to device coordinates.
  auto scale = GetDpiScale();
  Point origin(rect.left() * scale, rect.top() * scale);
  Size size(rect.width() * scale, rect.height() * scale);
  UpdateCursorRect(Rect(origin, size));
}

void appcodeWindow::OnResetImeComposing() {
  AbortImeComposing();
}

bool appcodeWindow::OnBitmapSurfaceCleared() {
  HDC dc = ::GetDC(GetWindowHandle());
  bool result = ::PatBlt(dc, 0, 0, current_width_, current_height_, BLACKNESS);
  ::ReleaseDC(GetWindowHandle(), dc);
  return result;
}

bool appcodeWindow::OnBitmapSurfaceUpdated(const void* allocation,
                                           size_t row_bytes,
                                           size_t height) {
  HDC dc = ::GetDC(GetWindowHandle());
  BITMAPINFO bmi = {};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = row_bytes / 4;
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  int ret = ::SetDIBitsToDevice(dc, 0, 0, row_bytes / 4, height, 0, 0, 0,
                                height, allocation, &bmi, DIB_RGB_COLORS);
  ::ReleaseDC(GetWindowHandle(), dc);
  return ret != 0;
}

gfx::NativeViewAccessible appcodeWindow::GetNativeViewAccessible() {
  if (binding_handler_delegate_ == nullptr) {
    return nullptr;
  }

  return binding_handler_delegate_->GetNativeViewAccessible();
}

PointerLocation appcodeWindow::GetPrimaryPointerLocation() {
  POINT point;
  GetCursorPos(&point);
  ScreenToClient(GetWindowHandle(), &point);
  return {(size_t)point.x, (size_t)point.y};
}

void appcodeWindow::OnThemeChange() {
  binding_handler_delegate_->OnHighContrastChanged();
}

ui::AXFragmentRootDelegateWin* appcodeWindow::GetAxFragmentRootDelegate() {
  return binding_handler_delegate_->GetAxFragmentRootDelegate();
}

AlertPlatformNodeDelegate* appcodeWindow::GetAlertDelegate() {
  CreateAxFragmentRoot();
  return alert_delegate_.get();
}

ui::AXPlatformNodeWin* appcodeWindow::GetAlert() {
  CreateAxFragmentRoot();
  return alert_node_.get();
}

void appcodeWindow::OnWindowStateEvent(WindowStateEvent event) {
  switch (event) {
    case WindowStateEvent::kShow:
      restored_ = true;
      break;
    case WindowStateEvent::kHide:
      restored_ = false;
      focused_ = false;
      break;
    case WindowStateEvent::kFocus:
      focused_ = true;
      if (binding_handler_delegate_) {
        binding_handler_delegate_->OnFocus(
            appcodeViewFocusState::kFocused,
            appcodeViewFocusDirection::kUndefined);
      }
      break;
    case WindowStateEvent::kUnfocus:
      focused_ = false;
      if (binding_handler_delegate_) {
        binding_handler_delegate_->OnFocus(
            appcodeViewFocusState::kUnfocused,
            appcodeViewFocusDirection::kUndefined);
      }
      break;
  }
  HWND hwnd = GetWindowHandle();
  if (hwnd && binding_handler_delegate_) {
    binding_handler_delegate_->OnWindowStateEvent(hwnd, event);
  }
}

void appcodeWindow::TrackMouseLeaveEvent(HWND hwnd) {
  if (!tracking_mouse_leave_) {
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = hwnd;
    tme.dwFlags = TME_LEAVE;
    TrackMouseEvent(&tme);
    tracking_mouse_leave_ = true;
  }
}

void appcodeWindow::HandleResize(UINT width, UINT height) {
  current_width_ = width;
  current_height_ = height;
  if (direct_manipulation_owner_) {
    direct_manipulation_owner_->ResizeViewport(width, height);
  }
  OnResize(width, height);
}

appcodeWindow* appcodeWindow::GetThisFromHandle(HWND const window) noexcept {
  return reinterpret_cast<appcodeWindow*>(
      GetWindowLongPtr(window, GWLP_USERDATA));
}

void appcodeWindow::UpdateScrollOffsetMultiplier() {
  UINT lines_per_scroll = kLinesPerScrollWindowsDefault;

  // Get lines per scroll wheel value from Windows
  SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lines_per_scroll, 0);

  // This logic is based off Chromium's implementation
  // https://source.chromium.org/chromium/chromium/src/+/main:ui/events/blink/web_input_event_builders_win.cc;l=319-331
  scroll_offset_multiplier_ =
      static_cast<float>(lines_per_scroll) * 100.0 / 3.0;
}

void appcodeWindow::InitializeChild(const char* title,
                                    unsigned int width,
                                    unsigned int height) {
  Destroy();
  std::wstring converted_title = NarrowToWide(title);

  WNDCLASS window_class = RegisterWindowClass(converted_title);

  auto* result = CreateWindowEx(
      0, window_class.lpszClassName, converted_title.c_str(),
      WS_CHILD | WS_VISIBLE, CW_DEFAULT, CW_DEFAULT, width, height,
      HWND_MESSAGE, nullptr, window_class.hInstance, this);

  if (result == nullptr) {
    auto error = GetLastError();
    LPWSTR message = nullptr;
    size_t size = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&message), 0, NULL);
    OutputDebugString(message);
    LocalFree(message);
  }
  SetUserObjectInformationA(GetCurrentProcess(),
                            UOI_TIMERPROC_EXCEPTION_SUPPRESSION, FALSE, 1);
  // SetTimer is not precise, if a 16 ms interval is requested, it will instead
  // often fire in an interval of 32 ms. Providing a value of 14 will ensure it
  // runs every 16 ms, which will allow for 60 Hz trackpad gesture events, which
  // is the maximal frequency supported by SetTimer.
  SetTimer(result, kDirectManipulationTimer, 14, nullptr);
  direct_manipulation_owner_ = std::make_unique<DirectManipulationOwner>(this);
  direct_manipulation_owner_->Init(width, height);
}

HWND appcodeWindow::GetWindowHandle() {
  return window_handle_;
}

BOOL appcodeWindow::Win32PeekMessage(LPMSG lpMsg,
                                     UINT wMsgFilterMin,
                                     UINT wMsgFilterMax,
                                     UINT wRemoveMsg) {
  return ::PeekMessage(lpMsg, window_handle_, wMsgFilterMin, wMsgFilterMax,
                       wRemoveMsg);
}

uint32_t appcodeWindow::Win32MapVkToChar(uint32_t virtual_key) {
  return ::MapVirtualKey(virtual_key, MAPVK_VK_TO_CHAR);
}

UINT appcodeWindow::Win32DispatchMessage(UINT Msg,
                                         WPARAM wParam,
                                         LPARAM lParam) {
  return ::SendMessage(window_handle_, Msg, wParam, lParam);
}

std::wstring appcodeWindow::NarrowToWide(const char* source) {
  size_t length = strlen(source);
  size_t outlen = 0;
  std::wstring wideTitle(length, L'#');
  mbstowcs_s(&outlen, &wideTitle[0], length + 1, source, length);
  return wideTitle;
}

WNDCLASS appcodeWindow::RegisterWindowClass(std::wstring& title) {
  window_class_name_ = title;

  WNDCLASS window_class{};
  window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
  window_class.lpszClassName = title.c_str();
  window_class.style = CS_HREDRAW | CS_VREDRAW;
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.hInstance = GetModuleHandle(nullptr);
  window_class.hIcon = nullptr;
  window_class.hbrBackground = 0;
  window_class.lpszMenuName = nullptr;
  window_class.lpfnWndProc = WndProc;
  RegisterClass(&window_class);
  return window_class;
}

LRESULT CALLBACK appcodeWindow::WndProc(HWND const window,
                                        UINT const message,
                                        WPARAM const wparam,
                                        LPARAM const lparam) noexcept {
  if (message == WM_NCCREATE) {
    auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
    SetWindowLongPtr(window, GWLP_USERDATA,
                     reinterpret_cast<LONG_PTR>(cs->lpCreateParams));

    auto that = static_cast<appcodeWindow*>(cs->lpCreateParams);
    that->window_handle_ = window;
    that->text_input_manager_->SetWindowHandle(window);
    RegisterTouchWindow(window, 0);
  } else if (appcodeWindow* that = GetThisFromHandle(window)) {
    return that->HandleMessage(message, wparam, lparam);
  }

  return DefWindowProc(window, message, wparam, lparam);
}

LRESULT
appcodeWindow::HandleMessage(UINT const message,
                             WPARAM const wparam,
                             LPARAM const lparam) noexcept {
  LPARAM result_lparam = lparam;
  int xPos = 0, yPos = 0;
  UINT width = 0, height = 0;
  UINT button_pressed = 0;
  appcodePointerDeviceKind device_kind;

  switch (message) {
    case kWmDpiChangedBeforeParent:
      current_dpi_ = GetDpiForHWND(window_handle_);
      OnDpiScale(current_dpi_);
      return 0;
    case WM_SIZE:
      width = LOWORD(lparam);
      height = HIWORD(lparam);

      current_width_ = width;
      current_height_ = height;
      HandleResize(width, height);

      OnWindowStateEvent(width == 0 && height == 0 ? WindowStateEvent::kHide
                                                   : WindowStateEvent::kShow);
      break;
    case WM_PAINT:
      OnPaint();
      break;
    case WM_TOUCH: {
      UINT num_points = LOWORD(wparam);
      touch_points_.resize(num_points);
      auto touch_input_handle = reinterpret_cast<HTOUCHINPUT>(lparam);
      if (GetTouchInputInfo(touch_input_handle, num_points,
                            touch_points_.data(), sizeof(TOUCHINPUT))) {
        for (const auto& touch : touch_points_) {
          // Generate a mapped ID for the Windows-provided touch ID
          auto touch_id = touch_id_generator_.GetGeneratedId(touch.dwID);

          POINT pt = {TOUCH_COORD_TO_PIXEL(touch.x),
                      TOUCH_COORD_TO_PIXEL(touch.y)};
          ScreenToClient(window_handle_, &pt);
          auto x = static_cast<double>(pt.x);
          auto y = static_cast<double>(pt.y);

          if (touch.dwFlags & TOUCHEVENTF_DOWN) {
            OnPointerDown(x, y, kappcodePointerDeviceKindTouch, touch_id,
                          WM_LBUTTONDOWN);
          } else if (touch.dwFlags & TOUCHEVENTF_MOVE) {
            OnPointerMove(x, y, kappcodePointerDeviceKindTouch, touch_id, 0);
          } else if (touch.dwFlags & TOUCHEVENTF_UP) {
            OnPointerUp(x, y, kappcodePointerDeviceKindTouch, touch_id,
                        WM_LBUTTONDOWN);
            OnPointerLeave(x, y, kappcodePointerDeviceKindTouch, touch_id);
            touch_id_generator_.ReleaseNumber(touch.dwID);
          }
        }
        CloseTouchInputHandle(touch_input_handle);
      }
      return 0;
    }
    case WM_MOUSEMOVE:
      device_kind = GetappcodePointerDeviceKind();
      if (device_kind == kappcodePointerDeviceKindMouse) {
        TrackMouseLeaveEvent(window_handle_);

        xPos = GET_X_LPARAM(lparam);
        yPos = GET_Y_LPARAM(lparam);
        mouse_x_ = static_cast<double>(xPos);
        mouse_y_ = static_cast<double>(yPos);

        int mods = 0;
        if (wparam & MK_CONTROL) {
          mods |= kControl;
        }
        if (wparam & MK_SHIFT) {
          mods |= kShift;
        }
        OnPointerMove(mouse_x_, mouse_y_, device_kind, kDefaultPointerDeviceId,
                      mods);
      }
      break;
    case WM_MOUSELEAVE:
      device_kind = GetappcodePointerDeviceKind();
      if (device_kind == kappcodePointerDeviceKindMouse) {
        OnPointerLeave(mouse_x_, mouse_y_, device_kind,
                       kDefaultPointerDeviceId);
      }

      // Once the tracked event is received, the TrackMouseEvent function
      // resets. Set to false to make sure it's called once mouse movement is
      // detected again.
      tracking_mouse_leave_ = false;
      break;
    case WM_SETCURSOR: {
      UINT hit_test_result = LOWORD(lparam);
      if (hit_test_result == HTCLIENT) {
        // Halt further processing to prevent DefWindowProc from setting the
        // cursor back to the registered class cursor.
        return TRUE;
      }
      break;
    }
    case WM_SETFOCUS:
      OnWindowStateEvent(WindowStateEvent::kFocus);
      ::CreateCaret(window_handle_, nullptr, 1, 1);
      break;
    case WM_KILLFOCUS:
      OnWindowStateEvent(WindowStateEvent::kUnfocus);
      ::DestroyCaret();
      break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
      device_kind = GetappcodePointerDeviceKind();
      if (device_kind != kappcodePointerDeviceKindMouse) {
        break;
      }

      if (message == WM_LBUTTONDOWN) {
        // Capture the pointer in case the user drags outside the client area.
        // In this case, the "mouse leave" event is delayed until the user
        // releases the button. It's only activated on left click given that
        // it's more common for apps to handle dragging with only the left
        // button.
        SetCapture(window_handle_);
      }
      button_pressed = message;
      if (message == WM_XBUTTONDOWN) {
        button_pressed = GET_XBUTTON_WPARAM(wparam);
      }
      xPos = GET_X_LPARAM(lparam);
      yPos = GET_Y_LPARAM(lparam);
      OnPointerDown(static_cast<double>(xPos), static_cast<double>(yPos),
                    device_kind, kDefaultPointerDeviceId, button_pressed);
      break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
      device_kind = GetappcodePointerDeviceKind();
      if (device_kind != kappcodePointerDeviceKindMouse) {
        break;
      }

      if (message == WM_LBUTTONUP) {
        ReleaseCapture();
      }
      button_pressed = message;
      if (message == WM_XBUTTONUP) {
        button_pressed = GET_XBUTTON_WPARAM(wparam);
      }
      xPos = GET_X_LPARAM(lparam);
      yPos = GET_Y_LPARAM(lparam);
      OnPointerUp(static_cast<double>(xPos), static_cast<double>(yPos),
                  device_kind, kDefaultPointerDeviceId, button_pressed);
      break;
    case WM_MOUSEWHEEL:
      OnScroll(0.0,
               -(static_cast<short>(HIWORD(wparam)) /
                 static_cast<double>(WHEEL_DELTA)),
               kappcodePointerDeviceKindMouse, kDefaultPointerDeviceId);
      break;
    case WM_MOUSEHWHEEL:
      OnScroll((static_cast<short>(HIWORD(wparam)) /
                static_cast<double>(WHEEL_DELTA)),
               0.0, kappcodePointerDeviceKindMouse, kDefaultPointerDeviceId);
      break;
    case WM_GETOBJECT: {
      LRESULT lresult = OnGetObject(message, wparam, lparam);
      if (lresult) {
        return lresult;
      }
      break;
    }
    case WM_TIMER:
      if (wparam == kDirectManipulationTimer) {
        direct_manipulation_owner_->Update();
        return 0;
      }
      break;
    case DM_POINTERHITTEST: {
      if (direct_manipulation_owner_) {
        UINT contact_id = GET_POINTERID_WPARAM(wparam);
        POINTER_INPUT_TYPE pointer_type;
        if (windows_proc_table_->GetPointerType(contact_id, &pointer_type) &&
            pointer_type == PT_TOUCHPAD) {
          direct_manipulation_owner_->SetContact(contact_id);
        }
      }
      break;
    }
    case WM_INPUTLANGCHANGE:
      // TODO(cbracken): pass this to TextInputManager to aid with
      // language-specific issues.
      break;
    case WM_IME_SETCONTEXT:
      OnImeSetContext(message, wparam, lparam);
      // Strip the ISC_SHOWUICOMPOSITIONWINDOW bit from lparam before passing it
      // to DefWindowProc() so that the composition window is hidden since
      // appcode renders the composing string itself.
      result_lparam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
      break;
    case WM_IME_STARTCOMPOSITION:
      OnImeStartComposition(message, wparam, lparam);
      // Suppress further processing by DefWindowProc() so that the default
      // system IME style isn't used, but rather the one set in the
      // WM_IME_SETCONTEXT handler.
      return TRUE;
    case WM_IME_COMPOSITION:
      OnImeComposition(message, wparam, lparam);
      if (lparam & GCS_RESULTSTR || lparam & GCS_COMPSTR) {
        // Suppress further processing by DefWindowProc() since otherwise it
        // will emit the result string as WM_CHAR messages on commit. Instead,
        // committing the composing text to the EditableText string is handled
        // in TextInputModel::CommitComposing, triggered by
        // OnImeEndComposition().
        return TRUE;
      }
      break;
    case WM_IME_ENDCOMPOSITION:
      OnImeEndComposition(message, wparam, lparam);
      return TRUE;
    case WM_IME_REQUEST:
      OnImeRequest(message, wparam, lparam);
      break;
    case WM_UNICHAR: {
      // Tell third-pary app, we can support Unicode.
      if (wparam == UNICODE_NOCHAR)
        return TRUE;
      // DefWindowProc will send WM_CHAR for this WM_UNICHAR.
      break;
    }
    case WM_THEMECHANGED:
      OnThemeChange();
      break;
    case WM_DEADCHAR:
    case WM_SYSDEADCHAR:
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
      if (keyboard_manager_->HandleMessage(message, wparam, lparam)) {
        return 0;
      }
      break;
  }

  return Win32DefWindowProc(window_handle_, message, wparam, result_lparam);
}

LRESULT appcodeWindow::OnGetObject(UINT const message,
                                   WPARAM const wparam,
                                   LPARAM const lparam) {
  LRESULT reference_result = static_cast<LRESULT>(0L);

  // Only the lower 32 bits of lparam are valid when checking the object id
  // because it sometimes gets sign-extended incorrectly (but not always).
  DWORD obj_id = static_cast<DWORD>(static_cast<DWORD_PTR>(lparam));

  bool is_uia_request = static_cast<DWORD>(UiaRootObjectId) == obj_id;
  bool is_msaa_request = static_cast<DWORD>(OBJID_CLIENT) == obj_id;

  if (is_uia_request || is_msaa_request) {
    // On Windows, we don't get a notification that the screen reader has been
    // enabled or disabled. There is an API to query for screen reader state,
    // but that state isn't set by all screen readers, including by Narrator,
    // the screen reader that ships with Windows:
    // https://docs.microsoft.com/en-us/windows/win32/winauto/screen-reader-parameter
    //
    // Instead, we enable semantics in appcode if Windows issues queries for
    // Microsoft Active Accessibility (MSAA) COM objects.
    OnUpdateSemanticsEnabled(true);
  }

  gfx::NativeViewAccessible root_view = GetNativeViewAccessible();
  // TODO(schectman): UIA is currently disabled by default.
  // https://github.com/appcode/appcode/issues/114547
  if (root_view) {
    CreateAxFragmentRoot();
    if (is_uia_request) {
#ifdef appcode_ENGINE_USE_UIA
      // Retrieve UIA object for the root view.
      Microsoft::WRL::ComPtr<IRawElementProviderSimple> root;
      if (SUCCEEDED(
              ax_fragment_root_->GetNativeViewAccessible()->QueryInterface(
                  IID_PPV_ARGS(&root)))) {
        // Return the UIA object via UiaReturnRawElementProvider(). See:
        // https://docs.microsoft.com/en-us/windows/win32/winauto/wm-getobject
        reference_result = UiaReturnRawElementProvider(window_handle_, wparam,
                                                       lparam, root.Get());
      } else {
        FML_LOG(ERROR) << "Failed to query AX fragment root.";
      }
#endif  // appcode_ENGINE_USE_UIA
    } else if (is_msaa_request) {
      // Create the accessibility root if it does not already exist.
      // Return the IAccessible for the root view.
      Microsoft::WRL::ComPtr<IAccessible> root;
      ax_fragment_root_->GetNativeViewAccessible()->QueryInterface(
          IID_PPV_ARGS(&root));
      reference_result = LresultFromObject(IID_IAccessible, wparam, root.Get());
    }
  }
  return reference_result;
}

void appcodeWindow::OnImeSetContext(UINT const message,
                                    WPARAM const wparam,
                                    LPARAM const lparam) {
  if (wparam != 0) {
    text_input_manager_->CreateImeWindow();
  }
}

void appcodeWindow::OnImeStartComposition(UINT const message,
                                          WPARAM const wparam,
                                          LPARAM const lparam) {
  text_input_manager_->CreateImeWindow();
  OnComposeBegin();
}

void appcodeWindow::OnImeComposition(UINT const message,
                                     WPARAM const wparam,
                                     LPARAM const lparam) {
  // Update the IME window position.
  text_input_manager_->UpdateImeWindow();

  if (lparam == 0) {
    OnComposeChange(u"", 0);
    OnComposeCommit();
  }

  // Process GCS_RESULTSTR at fisrt, because Google Japanese Input and ATOK send
  // both GCS_RESULTSTR and GCS_COMPSTR to commit composed text and send new
  // composing text.
  if (lparam & GCS_RESULTSTR) {
    // Commit but don't end composing.
    // Read the committed composing string.
    long pos = text_input_manager_->GetComposingCursorPosition();
    std::optional<std::u16string> text = text_input_manager_->GetResultString();
    if (text) {
      OnComposeChange(text.value(), pos);
      OnComposeCommit();
    }
  }
  if (lparam & GCS_COMPSTR) {
    // Read the in-progress composing string.
    long pos = text_input_manager_->GetComposingCursorPosition();
    std::optional<std::u16string> text =
        text_input_manager_->GetComposingString();
    if (text) {
      OnComposeChange(text.value(), pos);
    }
  }
}

void appcodeWindow::OnImeEndComposition(UINT const message,
                                        WPARAM const wparam,
                                        LPARAM const lparam) {
  text_input_manager_->DestroyImeWindow();
  OnComposeEnd();
}

void appcodeWindow::OnImeRequest(UINT const message,
                                 WPARAM const wparam,
                                 LPARAM const lparam) {
  // TODO(cbracken): Handle IMR_RECONVERTSTRING, IMR_DOCUMENTFEED,
  // and IMR_QUERYCHARPOSITION messages.
  // https://github.com/appcode/appcode/issues/74547
}

void appcodeWindow::AbortImeComposing() {
  text_input_manager_->AbortComposing();
}

void appcodeWindow::UpdateCursorRect(const Rect& rect) {
  text_input_manager_->UpdateCaretRect(rect);
}

UINT appcodeWindow::GetCurrentDPI() {
  return current_dpi_;
}

UINT appcodeWindow::GetCurrentWidth() {
  return current_width_;
}

UINT appcodeWindow::GetCurrentHeight() {
  return current_height_;
}

float appcodeWindow::GetScrollOffsetMultiplier() {
  return scroll_offset_multiplier_;
}

LRESULT appcodeWindow::Win32DefWindowProc(HWND hWnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
  return ::DefWindowProc(hWnd, Msg, wParam, lParam);
}

void appcodeWindow::Destroy() {
  if (window_handle_) {
    text_input_manager_->SetWindowHandle(nullptr);
    DestroyWindow(window_handle_);
    window_handle_ = nullptr;
  }

  UnregisterClass(window_class_name_.c_str(), nullptr);
}

void appcodeWindow::CreateAxFragmentRoot() {
  if (ax_fragment_root_) {
    return;
  }
  ax_fragment_root_ = std::make_unique<ui::AXFragmentRootWin>(
      window_handle_, GetAxFragmentRootDelegate());
  alert_delegate_ =
      std::make_unique<AlertPlatformNodeDelegate>(*ax_fragment_root_);
  ui::AXPlatformNode* alert_node =
      ui::AXPlatformNodeWin::Create(alert_delegate_.get());
  alert_node_.reset(static_cast<ui::AXPlatformNodeWin*>(alert_node));
  ax_fragment_root_->SetAlertNode(alert_node_.get());
}

}  // namespace appcode
