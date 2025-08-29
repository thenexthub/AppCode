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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_KEYBOARD_KEY_CHANNEL_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_KEYBOARD_KEY_CHANNEL_HANDLER_H_

#include <deque>
#include <memory>
#include <string>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/basic_message_channel.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/binary_messenger.h"
#include "appcode/shell/platform/windows/keyboard_key_handler.h"
#include "rapidjson/document.h"

namespace appcode {

// A delegate of |KeyboardKeyHandler| that handles events by sending the
// raw information through the method channel.
//
// This class communicates with the RawKeyboard API in the framework.
class KeyboardKeyChannelHandler
    : public KeyboardKeyHandler::KeyboardKeyHandlerDelegate {
 public:
  // Create a |KeyboardKeyChannelHandler| by specifying the messenger
  // through which the events are sent.
  explicit KeyboardKeyChannelHandler(appcode::BinaryMessenger* messenger);

  ~KeyboardKeyChannelHandler();

  // |KeyboardKeyHandler::KeyboardKeyHandlerDelegate|
  void KeyboardHook(int key,
                    int scancode,
                    int action,
                    char32_t character,
                    bool extended,
                    bool was_down,
                    std::function<void(bool)> callback);

  void SyncModifiersIfNeeded(int modifiers_state);

  std::map<uint64_t, uint64_t> GetPressedState();

 private:
  // The Flutter system channel for key event messages.
  std::unique_ptr<appcode::BasicMessageChannel<rapidjson::Document>> channel_;

  FML_DISALLOW_COPY_AND_ASSIGN(KeyboardKeyChannelHandler);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_KEYBOARD_KEY_CHANNEL_HANDLER_H_
