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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_TEST_BINARY_MESSENGER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_TEST_BINARY_MESSENGER_H_

#include <functional>
#include <map>
#include <string>

#include "appcode/fml/logging.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/binary_messenger.h"

namespace appcode {

// A trivial BinaryMessenger implementation for use in tests.
class TestBinaryMessenger : public BinaryMessenger {
 public:
  using SendHandler = std::function<void(const std::string& channel,
                                         const uint8_t* message,
                                         size_t message_size,
                                         BinaryReply reply)>;

  // Creates a new messenge that forwards all calls to |send_handler|.
  explicit TestBinaryMessenger(SendHandler send_handler = nullptr)
      : send_handler_(std::move(send_handler)) {}

  virtual ~TestBinaryMessenger() = default;

  // Simulates a message from the engine on the given channel.
  //
  // Returns false if no handler is registered on that channel.
  bool SimulateEngineMessage(const std::string& channel,
                             const uint8_t* message,
                             size_t message_size,
                             BinaryReply reply) {
    auto handler = registered_handlers_.find(channel);
    if (handler == registered_handlers_.end()) {
      return false;
    }
    (handler->second)(message, message_size, reply);
    return true;
  }

  // |appcode::BinaryMessenger|
  void Send(const std::string& channel,
            const uint8_t* message,
            size_t message_size,
            BinaryReply reply) const override {
    // If something under test sends a message, the test should be handling it.
    FML_DCHECK(send_handler_);
    send_handler_(channel, message, message_size, reply);
  }

  // |appcode::BinaryMessenger|
  void SetMessageHandler(const std::string& channel,
                         BinaryMessageHandler handler) override {
    if (handler) {
      registered_handlers_[channel] = handler;
    } else {
      registered_handlers_.erase(channel);
    }
  }

 private:
  // Handler to call for SendMessage.
  SendHandler send_handler_;

  // Mapping of channel name to registered handlers.
  std::map<std::string, BinaryMessageHandler> registered_handlers_;

  FML_DISALLOW_COPY_AND_ASSIGN(TestBinaryMessenger);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_TEST_BINARY_MESSENGER_H_
