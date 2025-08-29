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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_KEYBOARD_HOOK_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_GLFW_KEYBOARD_HOOK_HANDLER_H_

#include <GLFW/glfw3.h>

#include "appcode/shell/platform/glfw/public/appcode_glfw.h"

namespace appcode {

// Abstract class for handling keyboard input events.
class KeyboardHookHandler {
 public:
  virtual ~KeyboardHookHandler() = default;

  // A function for hooking into keyboard input.
  virtual void KeyboardHook(GLFWwindow* window,
                            int key,
                            int scancode,
                            int action,
                            int mods) = 0;

  // A function for hooking into unicode code point input.
  virtual void CharHook(GLFWwindow* window, unsigned int code_point) = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_GLFW_KEYBOARD_HOOK_HANDLER_H_
