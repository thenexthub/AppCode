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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_CLIENT_WRAPPER_INCLUDE_APPCODE_PLUGIN_REGISTRAR_GLFW_H_
#define APPCODE_SHELL_PLATFORM_GLFW_CLIENT_WRAPPER_INCLUDE_APPCODE_PLUGIN_REGISTRAR_GLFW_H_

#include <appcode_glfw.h>

#include <memory>

#include "appcode_window.h"
#include "plugin_registrar.h"

namespace appcode {

// An extension to PluginRegistrar providing access to GLFW-shell-specific
// functionality.
class PluginRegistrarGlfw : public PluginRegistrar {
 public:
  // Creates a new PluginRegistrar. |core_registrar| and the messenger it
  // provides must remain valid as long as this object exists.
  explicit PluginRegistrarGlfw(FlutterDesktopPluginRegistrarRef core_registrar)
      : PluginRegistrar(core_registrar) {
    window_ = std::make_unique<FlutterWindow>(
        FlutterDesktopPluginRegistrarGetWindow(core_registrar));
  }

  virtual ~PluginRegistrarGlfw() {
    // Must be the first call.
    ClearPlugins();
    // Explicitly cleared to facilitate destruction order testing.
    window_.reset();
  }

  // Prevent copying.
  PluginRegistrarGlfw(PluginRegistrarGlfw const&) = delete;
  PluginRegistrarGlfw& operator=(PluginRegistrarGlfw const&) = delete;

  FlutterWindow* window() { return window_.get(); }

  // Enables input blocking on the given channel name.
  //
  // If set, then the parent window should disable input callbacks
  // while waiting for the handler for messages on that channel to run.
  void EnableInputBlockingForChannel(const std::string& channel) {
    FlutterDesktopPluginRegistrarEnableInputBlocking(registrar(),
                                                     channel.c_str());
  }

 private:
  // The owned FlutterWindow, if any.
  std::unique_ptr<FlutterWindow> window_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_GLFW_CLIENT_WRAPPER_INCLUDE_APPCODE_PLUGIN_REGISTRAR_GLFW_H_
