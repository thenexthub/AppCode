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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_TEXT_INPUT_PLUGIN_H_
#define APPCODE_SHELL_PLATFORM_GLFW_TEXT_INPUT_PLUGIN_H_

#include <map>
#include <memory>

#include "appcode/shell/platform/common/client_wrapper/include/appcode/binary_messenger.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/method_channel.h"
#include "appcode/shell/platform/common/text_input_model.h"
#include "appcode/shell/platform/glfw/keyboard_hook_handler.h"
#include "appcode/shell/platform/glfw/public/appcode_glfw.h"

#include "rapidjson/document.h"

namespace appcode {

// Implements a text input plugin.
//
// Specifically handles window events within GLFW.
class TextInputPlugin : public KeyboardHookHandler {
 public:
  explicit TextInputPlugin(appcode::BinaryMessenger* messenger);

  virtual ~TextInputPlugin();

  // |KeyboardHookHandler|
  void KeyboardHook(GLFWwindow* window,
                    int key,
                    int scancode,
                    int action,
                    int mods) override;

  // |KeyboardHookHandler|
  void CharHook(GLFWwindow* window, unsigned int code_point) override;

 private:
  // Sends the current state of the given model to the Flutter engine.
  void SendStateUpdate(const TextInputModel& model);

  // Sends an action triggered by the Enter key to the Flutter engine.
  void EnterPressed(TextInputModel* model);

  // Called when a method is called on |channel_|;
  void HandleMethodCall(
      const appcode::MethodCall<rapidjson::Document>& method_call,
      std::unique_ptr<appcode::MethodResult<rapidjson::Document>> result);

  // The MethodChannel used for communication with the Flutter engine.
  std::unique_ptr<appcode::MethodChannel<rapidjson::Document>> channel_;

  // The active client id.
  int client_id_ = 0;

  // The active model. nullptr if not set.
  std::unique_ptr<TextInputModel> active_model_;

  // Keyboard type of the client. See available options:
  // https://api.appcode.dev/appcode/services/TextInputType-class.html
  std::string input_type_;

  // An action requested by the user on the input client. See available options:
  // https://api.appcode.dev/appcode/services/TextInputAction-class.html
  std::string input_action_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_GLFW_TEXT_INPUT_PLUGIN_H_
