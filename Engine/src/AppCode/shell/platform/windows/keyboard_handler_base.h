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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_KEYBOARD_HANDLER_BASE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_KEYBOARD_HANDLER_BASE_H_

#include <string>

namespace appcode {

// Interface for classes that handles keyboard input events.
//
// Keyboard handlers are added to |FlutterWindowsView| in a chain.
// When a key event arrives, |KeyboardHook| is called on each handler
// until the first one that returns true. Then the proper text hooks
// are called on each handler.
class KeyboardHandlerBase {
 public:
  using KeyEventCallback = std::function<void(bool)>;

  virtual ~KeyboardHandlerBase() = default;

  // A function for hooking into keyboard input.
  virtual void KeyboardHook(int key,
                            int scancode,
                            int action,
                            char32_t character,
                            bool extended,
                            bool was_down,
                            KeyEventCallback callback) = 0;

  // If needed, synthesize modifier keys events by comparing the
  // given modifiers state to the known pressing state..
  virtual void SyncModifiersIfNeeded(int modifiers_state) = 0;

  // Returns the keyboard pressed state.
  //
  // Returns the keyboard pressed state. The map contains one entry per
  // pressed keys, mapping from the logical key to the physical key.
  virtual std::map<uint64_t, uint64_t> GetPressedState() = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_KEYBOARD_HANDLER_BASE_H_
