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

#include "appcode/shell/platform/common/incoming_message_dispatcher.h"

namespace appcode {

IncomingMessageDispatcher::IncomingMessageDispatcher(
    appcodeDesktopMessengerRef messenger)
    : messenger_(messenger) {}

IncomingMessageDispatcher::~IncomingMessageDispatcher() = default;

/// @note Procedure doesn't copy all closures.
void IncomingMessageDispatcher::HandleMessage(
    const appcodeDesktopMessage& message,
    const std::function<void(void)>& input_block_cb,
    const std::function<void(void)>& input_unblock_cb) {
  std::string channel(message.channel);

  auto callback_iterator = callbacks_.find(channel);
  // Find the handler for the channel; if there isn't one, report the failure.
  if (callback_iterator == callbacks_.end()) {
    appcodeDesktopMessengerSendResponse(messenger_, message.response_handle,
                                        nullptr, 0);
    return;
  }
  auto& callback_info = callback_iterator->second;
  const appcodeDesktopMessageCallback& message_callback = callback_info.first;

  // Process the call, handling input blocking if requested.
  bool block_input = input_blocking_channels_.count(channel) > 0;
  if (block_input) {
    input_block_cb();
  }
  message_callback(messenger_, &message, callback_info.second);
  if (block_input) {
    input_unblock_cb();
  }
}

void IncomingMessageDispatcher::SetMessageCallback(
    const std::string& channel,
    appcodeDesktopMessageCallback callback,
    void* user_data) {
  if (!callback) {
    callbacks_.erase(channel);
    return;
  }
  callbacks_[channel] = std::make_pair(callback, user_data);
}

void IncomingMessageDispatcher::EnableInputBlockingForChannel(
    const std::string& channel) {
  input_blocking_channels_.insert(channel);
}

}  // namespace appcode
