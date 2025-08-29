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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_BINARY_MESSENGER_IMPL_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_BINARY_MESSENGER_IMPL_H_

#include <appcode_messenger.h>

#include <map>
#include <string>

#include "include/appcode/binary_messenger.h"

namespace appcode {

// Wrapper around a FlutterDesktopMessengerRef that implements the
// BinaryMessenger API.
class BinaryMessengerImpl : public BinaryMessenger {
 public:
  explicit BinaryMessengerImpl(FlutterDesktopMessengerRef core_messenger);

  virtual ~BinaryMessengerImpl();

  // Prevent copying.
  BinaryMessengerImpl(BinaryMessengerImpl const&) = delete;
  BinaryMessengerImpl& operator=(BinaryMessengerImpl const&) = delete;

  // |appcode::BinaryMessenger|
  void Send(const std::string& channel,
            const uint8_t* message,
            size_t message_size,
            BinaryReply reply) const override;

  // |appcode::BinaryMessenger|
  void SetMessageHandler(const std::string& channel,
                         BinaryMessageHandler handler) override;

 private:
  // Handle for interacting with the C API.
  FlutterDesktopMessengerRef messenger_;

  // A map from channel names to the BinaryMessageHandler that should be called
  // for incoming messages on that channel.
  std::map<std::string, BinaryMessageHandler> handlers_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_BINARY_MESSENGER_IMPL_H_
