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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_KEY_EVENT_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_GLFW_KEY_EVENT_HANDLER_H_

#include <memory>

#include "appcode/shell/platform/common/client_wrapper/include/appcode/basic_message_channel.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/binary_messenger.h"
#include "appcode/shell/platform/glfw/keyboard_hook_handler.h"
#include "appcode/shell/platform/glfw/public/appcode_glfw.h"
#include "rapidjson/document.h"

namespace appcode {

// Implements a KeyboardHookHandler
//
// Handles key events and forwards them to the Flutter engine.
class KeyEventHandler : public KeyboardHookHandler {
 public:
  explicit KeyEventHandler(appcode::BinaryMessenger* messenger);

  virtual ~KeyEventHandler();

  // |KeyboardHookHandler|
  void KeyboardHook(GLFWwindow* window,
                    int key,
                    int scancode,
                    int action,
                    int mods) override;

  // |KeyboardHookHandler|
  void CharHook(GLFWwindow* window, unsigned int code_point) override;

 private:
  // The Flutter system channel for key event messages.
  std::unique_ptr<appcode::BasicMessageChannel<rapidjson::Document>> channel_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_GLFW_KEY_EVENT_HANDLER_H_
