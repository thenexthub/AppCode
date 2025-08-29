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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_CLIENT_WRAPPER_INCLUDE_APPCODE_APPCODE_VIEW_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_CLIENT_WRAPPER_INCLUDE_APPCODE_APPCODE_VIEW_H_

#include <appcode_windows.h>

namespace appcode {

// The unique identifier for a view.
typedef int64_t FlutterViewId;

// A view displaying Flutter content.
class FlutterView {
 public:
  explicit FlutterView(FlutterDesktopViewRef view) : view_(view) {}

  // Destroys this reference to the view. The underlying view is not destroyed.
  virtual ~FlutterView() = default;

  // Prevent copying.
  FlutterView(FlutterView const&) = delete;
  FlutterView& operator=(FlutterView const&) = delete;

  // Returns the backing HWND for the view.
  HWND GetNativeWindow() { return FlutterDesktopViewGetHWND(view_); }

  // Returns the DXGI adapter used for rendering or nullptr in case of error.
  IDXGIAdapter* GetGraphicsAdapter() {
    return FlutterDesktopViewGetGraphicsAdapter(view_);
  }

 private:
  // Handle for interacting with the C API's view.
  FlutterDesktopViewRef view_ = nullptr;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_CLIENT_WRAPPER_INCLUDE_APPCODE_APPCODE_VIEW_H_
