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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_CLIENT_WRAPPER_INCLUDE_APPCODE_APPCODE_WINDOW_H_
#define APPCODE_SHELL_PLATFORM_GLFW_CLIENT_WRAPPER_INCLUDE_APPCODE_APPCODE_WINDOW_H_

#include <appcode_glfw.h>

#include <string>
#include <vector>

#include "plugin_registrar.h"

namespace appcode {

// A data type for window position and size.
struct WindowFrame {
  int left;
  int top;
  int width;
  int height;
};

// A window displaying Flutter content.
class FlutterWindow {
 public:
  explicit FlutterWindow(FlutterDesktopWindowRef window) : window_(window) {}

  ~FlutterWindow() = default;

  // Prevent copying.
  FlutterWindow(FlutterWindow const&) = delete;
  FlutterWindow& operator=(FlutterWindow const&) = delete;

  // Enables or disables hover tracking.
  //
  // If hover is enabled, mouse movement will send hover events to the Flutter
  // engine, rather than only tracking the mouse while the button is pressed.
  // Defaults to off.
  void SetHoverEnabled(bool enabled) {
    FlutterDesktopWindowSetHoverEnabled(window_, enabled);
  }

  // Sets the displayed title of the window.
  void SetTitle(const std::string& title) {
    FlutterDesktopWindowSetTitle(window_, title.c_str());
  }

  // Sets the displayed icon for the window.
  //
  // The pixel format is 32-bit RGBA. The provided image data only needs to be
  // valid for the duration of the call to this method. Pass a nullptr to revert
  // to the default icon.
  void SetIcon(uint8_t* pixel_data, int width, int height) {
    FlutterDesktopWindowSetIcon(window_, pixel_data, width, height);
  }

  // Returns the frame of the window, including any decoration (e.g., title
  // bar), in screen coordinates.
  WindowFrame GetFrame() {
    WindowFrame frame = {};
    FlutterDesktopWindowGetFrame(window_, &frame.left, &frame.top, &frame.width,
                                 &frame.height);
    return frame;
  }

  // Set the frame of the window, including any decoration (e.g., title
  // bar), in screen coordinates.
  void SetFrame(const WindowFrame& frame) {
    FlutterDesktopWindowSetFrame(window_, frame.left, frame.top, frame.width,
                                 frame.height);
  }

  // Returns the number of pixels per screen coordinate for the window.
  //
  // Flutter uses pixel coordinates, so this is the ratio of positions and sizes
  // seen by Flutter as compared to the screen.
  double GetScaleFactor() {
    return FlutterDesktopWindowGetScaleFactor(window_);
  }

  // Forces a specific pixel ratio for Flutter rendering, rather than one
  // computed automatically from screen information.
  //
  // To clear a previously set override, pass an override value of zero.
  void SetPixelRatioOverride(double pixel_ratio) {
    FlutterDesktopWindowSetPixelRatioOverride(window_, pixel_ratio);
  }

  // Sets the min/max size of |appcode_window| in screen coordinates. Use
  // kFlutterDesktopDontCare for any dimension you wish to leave unconstrained.
  void SetSizeLimits(FlutterDesktopSize minimum_size,
                     FlutterDesktopSize maximum_size) {
    FlutterDesktopWindowSetSizeLimits(window_, minimum_size, maximum_size);
  }

 private:
  // Handle for interacting with the C API's window.
  //
  // Note: window_ is conceptually owned by the controller, not this object.
  FlutterDesktopWindowRef window_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_GLFW_CLIENT_WRAPPER_INCLUDE_APPCODE_APPCODE_WINDOW_H_
