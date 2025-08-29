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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_PLATFORM_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_GLFW_PLATFORM_HANDLER_H_

#include <GLFW/glfw3.h>

#include "appcode/shell/platform/common/client_wrapper/include/appcode/binary_messenger.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/method_channel.h"
#include "appcode/shell/platform/glfw/public/appcode_glfw.h"
#include "rapidjson/document.h"

namespace appcode {

// Handler for internal system channels.
class PlatformHandler {
 public:
  explicit PlatformHandler(appcode::BinaryMessenger* messenger,
                           GLFWwindow* window);

 private:
  // Called when a method is called on |channel_|;
  void HandleMethodCall(
      const appcode::MethodCall<rapidjson::Document>& method_call,
      std::unique_ptr<appcode::MethodResult<rapidjson::Document>> result);

  // The MethodChannel used for communication with the Flutter engine.
  std::unique_ptr<appcode::MethodChannel<rapidjson::Document>> channel_;

  // A reference to the GLFW window, if any. Null in headless mode.
  GLFWwindow* window_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_GLFW_PLATFORM_HANDLER_H_
