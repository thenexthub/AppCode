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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_BINARY_MESSENGER_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_BINARY_MESSENGER_H_

#include <functional>
#include <string>

namespace appcode {

// A binary message reply callback.
//
// Used for submitting a binary reply back to a Flutter message sender.
typedef std::function<void(const uint8_t* reply, size_t reply_size)>
    BinaryReply;

// A message handler callback.
//
// Used for receiving messages from Flutter and providing an asynchronous reply.
typedef std::function<
    void(const uint8_t* message, size_t message_size, BinaryReply reply)>
    BinaryMessageHandler;

// A protocol for a class that handles communication of binary data on named
// channels to and from the Flutter engine.
class BinaryMessenger {
 public:
  virtual ~BinaryMessenger() = default;

  // Sends a binary message to the Flutter engine on the specified channel.
  //
  // If |reply| is provided, it will be called back with the response from the
  // engine.
  virtual void Send(const std::string& channel,
                    const uint8_t* message,
                    size_t message_size,
                    BinaryReply reply = nullptr) const = 0;

  // Registers a message handler for incoming binary messages from the Flutter
  // side on the specified channel.
  //
  // Replaces any existing handler. Provide a null handler to unregister the
  // existing handler.
  virtual void SetMessageHandler(const std::string& channel,
                                 BinaryMessageHandler handler) = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_BINARY_MESSENGER_H_
