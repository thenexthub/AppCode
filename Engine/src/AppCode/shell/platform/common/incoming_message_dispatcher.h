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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_INCOMING_MESSAGE_DISPATCHER_H_
#define APPCODE_SHELL_PLATFORM_COMMON_INCOMING_MESSAGE_DISPATCHER_H_

#include <functional>
#include <map>
#include <set>
#include <string>
#include <utility>

#include "appcode/shell/platform/common/public/appcode_messenger.h"

namespace appcode {

// Manages per-channel registration of callbacks for handling messages from the
// Flutter engine, and dispatching incoming messages to those handlers.
class IncomingMessageDispatcher {
 public:
  // Creates a new IncomingMessageDispatcher. |messenger| must remain valid as
  // long as this object exists.
  explicit IncomingMessageDispatcher(FlutterDesktopMessengerRef messenger);

  virtual ~IncomingMessageDispatcher();

  // Prevent copying.
  IncomingMessageDispatcher(IncomingMessageDispatcher const&) = delete;
  IncomingMessageDispatcher& operator=(IncomingMessageDispatcher const&) =
      delete;

  // Routes |message| to the registered handler for its channel, if any.
  //
  // If input blocking has been enabled on that channel, wraps the call to the
  // handler with calls to the given callbacks to block and then unblock input.
  //
  // If no handler is registered for the message's channel, sends a
  // NotImplemented response to the engine.
  void HandleMessage(
      const FlutterDesktopMessage& message,
      const std::function<void(void)>& input_block_cb = [] {},
      const std::function<void(void)>& input_unblock_cb = [] {});

  // Registers a message callback for incoming messages from the Flutter
  // side on the specified channel. |callback| will be called with the message
  // and |user_data| any time a message arrives on that channel.
  //
  // Replaces any existing callback. Pass a null callback to unregister the
  // existing callback.
  void SetMessageCallback(const std::string& channel,
                          FlutterDesktopMessageCallback callback,
                          void* user_data);

  // Enables input blocking on the given channel name.
  //
  // If set, then the parent window should disable input callbacks
  // while waiting for the handler for messages on that channel to run.
  void EnableInputBlockingForChannel(const std::string& channel);

 private:
  // Handle for interacting with the C messaging API.
  FlutterDesktopMessengerRef messenger_;

  // A map from channel names to the FlutterDesktopMessageCallback that should
  // be called for incoming messages on that channel, along with the void* user
  // data to pass to it.
  std::map<std::string, std::pair<FlutterDesktopMessageCallback, void*>>
      callbacks_;

  // Channel names for which input blocking should be enabled during the call to
  // that channel's handler.
  std::set<std::string> input_blocking_channels_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_INCOMING_MESSAGE_DISPATCHER_H_
