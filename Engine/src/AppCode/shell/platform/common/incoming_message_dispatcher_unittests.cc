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

#include "gtest/gtest.h"

namespace appcode {
TEST(IncomingMessageDispatcher, SetHandle) {
  appcodeDesktopMessengerRef messenger =
      reinterpret_cast<appcodeDesktopMessengerRef>(0xfeedface);
  const uint8_t* message_data = reinterpret_cast<const uint8_t*>(0xcafebabe);
  auto dispatcher = std::make_unique<IncomingMessageDispatcher>(messenger);
  bool did_call = false;
  dispatcher->SetMessageCallback(
      "hello",
      [](appcodeDesktopMessengerRef messenger,
         const appcodeDesktopMessage* message, void* user_data) {
        EXPECT_EQ(messenger,
                  reinterpret_cast<appcodeDesktopMessengerRef>(0xfeedface));
        EXPECT_EQ(message->message,
                  reinterpret_cast<const uint8_t*>(0xcafebabe));
        EXPECT_EQ(message->message_size, 123u);
        *reinterpret_cast<bool*>(user_data) = true;
      },
      &did_call);
  appcodeDesktopMessage message = {
      .struct_size = sizeof(appcodeDesktopMessage),
      .channel = "hello",
      .message = message_data,
      .message_size = 123,
      .response_handle = nullptr,
  };
  dispatcher->HandleMessage(message);
  EXPECT_TRUE(did_call);
}

TEST(IncomingMessageDispatcher, BlockInputFalse) {
  appcodeDesktopMessengerRef messenger = nullptr;
  auto dispatcher = std::make_unique<IncomingMessageDispatcher>(messenger);
  bool did_call[3] = {false, false, false};
  dispatcher->SetMessageCallback(
      "hello",
      [](appcodeDesktopMessengerRef messenger,
         const appcodeDesktopMessage* message,
         void* user_data) { reinterpret_cast<bool*>(user_data)[0] = true; },
      &did_call);
  appcodeDesktopMessage message = {
      .struct_size = sizeof(appcodeDesktopMessage),
      .channel = "hello",
      .message = nullptr,
      .message_size = 0,
      .response_handle = nullptr,
  };
  dispatcher->HandleMessage(
      message, [&did_call] { did_call[1] = true; },
      [&did_call] { did_call[2] = true; });
  EXPECT_TRUE(did_call[0]);
  EXPECT_FALSE(did_call[1]);
  EXPECT_FALSE(did_call[2]);
}

TEST(IncomingMessageDispatcher, BlockInputTrue) {
  appcodeDesktopMessengerRef messenger = nullptr;
  auto dispatcher = std::make_unique<IncomingMessageDispatcher>(messenger);
  static int counter = 0;
  int did_call[3] = {-1, -1, -1};
  dispatcher->EnableInputBlockingForChannel("hello");
  dispatcher->SetMessageCallback(
      "hello",
      [](appcodeDesktopMessengerRef messenger,
         const appcodeDesktopMessage* message,
         void* user_data) { reinterpret_cast<int*>(user_data)[counter++] = 1; },
      &did_call);
  appcodeDesktopMessage message = {
      .struct_size = sizeof(appcodeDesktopMessage),
      .channel = "hello",
      .message = nullptr,
      .message_size = 0,
      .response_handle = nullptr,
  };
  dispatcher->HandleMessage(
      message, [&did_call] { did_call[counter++] = 0; },
      [&did_call] { did_call[counter++] = 2; });
  EXPECT_EQ(did_call[0], 0);
  EXPECT_EQ(did_call[1], 1);
  EXPECT_EQ(did_call[2], 2);
}

}  // namespace appcode
