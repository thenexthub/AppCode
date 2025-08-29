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

// Included first as it collides with the X11 headers.
#include "gtest/gtest.h"

#include "appcode/shell/platform/linux/public/appcode_linux/fl_event_channel.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_channel.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_standard_method_codec.h"
#include "appcode/shell/platform/linux/testing/fl_mock_binary_messenger.h"

// Checks we detect a listen event.
TEST(FlEventChannelTest, Listen) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlEventChannel) channel = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(
      channel,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        EXPECT_EQ(fl_value_get_type(args), FL_VALUE_TYPE_NULL);

        return static_cast<FlMethodErrorResponse*>(nullptr);
      },
      nullptr, nullptr, nullptr);

  gboolean called = FALSE;
  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "listen", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;

        EXPECT_TRUE(FL_IS_METHOD_SUCCESS_RESPONSE(response));
        FlValue* result = fl_method_success_response_get_result(
            FL_METHOD_SUCCESS_RESPONSE(response));
        EXPECT_EQ(fl_value_get_type(result), FL_VALUE_TYPE_NULL);
      },
      &called);
  EXPECT_TRUE(called);
}

// Checks we can generate a listen exception.
TEST(FlEventChannelTest, ListenException) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlEventChannel) channel = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(
      channel,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        return fl_method_error_response_new("LISTEN-ERROR",
                                            "LISTEN-ERROR-MESSAGE", nullptr);
      },
      nullptr, nullptr, nullptr);

  gboolean called = FALSE;
  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "listen", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;

        EXPECT_TRUE(FL_IS_METHOD_ERROR_RESPONSE(response));
        EXPECT_STREQ(fl_method_error_response_get_code(
                         FL_METHOD_ERROR_RESPONSE(response)),
                     "LISTEN-ERROR");
        EXPECT_STREQ(fl_method_error_response_get_message(
                         FL_METHOD_ERROR_RESPONSE(response)),
                     "LISTEN-ERROR-MESSAGE");
      },
      &called);
  EXPECT_TRUE(called);
}

// Checks we detect a cancel event.
TEST(FlEventChannelTest, Cancel) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlEventChannel) channel = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(
      channel, nullptr,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        EXPECT_EQ(fl_value_get_type(args), FL_VALUE_TYPE_NULL);

        return static_cast<FlMethodErrorResponse*>(nullptr);
      },
      nullptr, nullptr);

  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "listen", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);
  gboolean called = FALSE;
  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "cancel", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;

        EXPECT_TRUE(FL_IS_METHOD_SUCCESS_RESPONSE(response));
        FlValue* result = fl_method_success_response_get_result(
            FL_METHOD_SUCCESS_RESPONSE(response));
        EXPECT_EQ(fl_value_get_type(result), FL_VALUE_TYPE_NULL);
      },
      &called);
  EXPECT_TRUE(called);
}

// Checks we can generate a cancel exception.
TEST(FlEventChannelTest, CancelException) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlEventChannel) channel = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(
      channel, nullptr,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        return fl_method_error_response_new("CANCEL-ERROR",
                                            "CANCEL-ERROR-MESSAGE", nullptr);
      },
      nullptr, nullptr);

  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "listen", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);
  gboolean called = FALSE;
  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "cancel", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;

        EXPECT_TRUE(FL_IS_METHOD_ERROR_RESPONSE(response));
        EXPECT_STREQ(fl_method_error_response_get_code(
                         FL_METHOD_ERROR_RESPONSE(response)),
                     "CANCEL-ERROR");
        EXPECT_STREQ(fl_method_error_response_get_message(
                         FL_METHOD_ERROR_RESPONSE(response)),
                     "CANCEL-ERROR-MESSAGE");
      },
      &called);
  EXPECT_TRUE(called);
}

// Checks args are passed to listen/cancel.
TEST(FlEventChannelTest, Args) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlEventChannel) channel = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  int call_count = 0;
  fl_event_channel_set_stream_handlers(
      channel,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        int* call_count = static_cast<int*>(user_data);
        EXPECT_EQ(*call_count, 0);
        (*call_count)++;

        g_autoptr(FlValue) expected_args = fl_value_new_string("LISTEN-ARGS");
        EXPECT_TRUE(fl_value_equal(args, expected_args));

        return static_cast<FlMethodErrorResponse*>(nullptr);
      },
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        int* call_count = static_cast<int*>(user_data);
        EXPECT_EQ(*call_count, 1);
        (*call_count)++;

        g_autoptr(FlValue) expected_args = fl_value_new_string("CANCEL-ARGS");
        EXPECT_TRUE(fl_value_equal(args, expected_args));

        return static_cast<FlMethodErrorResponse*>(nullptr);
      },
      &call_count, nullptr);

  g_autoptr(FlValue) listen_args = fl_value_new_string("LISTEN-ARGS");
  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "listen", listen_args,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);
  g_autoptr(FlValue) cancel_args = fl_value_new_string("CANCEL-ARGS");
  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "cancel", cancel_args,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);

  EXPECT_EQ(call_count, 2);
}

// Checks can send events.
TEST(FlEventChannelTest, SendEvents) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  int event_count = 0;
  fl_mock_binary_messenger_set_standard_event_channel(
      messenger, "test",
      [](FlMockBinaryMessenger* messenger, FlValue* event, gpointer user_data) {
        int* event_count = static_cast<int*>(user_data);

        EXPECT_EQ(fl_value_get_type(event), FL_VALUE_TYPE_INT);
        EXPECT_EQ(fl_value_get_int(event), *event_count);

        (*event_count)++;
      },
      [](FlMockBinaryMessenger* messenger, const gchar* code,
         const gchar* message, FlValue* details, gpointer user_data) {},
      &event_count);

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlEventChannel) channel = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(
      channel,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        // Send some events.
        for (int i = 0; i < 5; i++) {
          g_autoptr(FlValue) event = fl_value_new_int(i);
          g_autoptr(GError) error = nullptr;
          EXPECT_TRUE(fl_event_channel_send(channel, event, nullptr, &error));
          EXPECT_EQ(error, nullptr);
        }

        return static_cast<FlMethodErrorResponse*>(nullptr);
      },
      nullptr, nullptr, nullptr);

  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "listen", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);
  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "cancel", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);

  EXPECT_EQ(event_count, 5);
}

// Check can register an event channel with the same name as one previously
// used.
TEST(FlEventChannelTest, ReuseChannel) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  int event_count = 0;
  fl_mock_binary_messenger_set_standard_event_channel(
      messenger, "test",
      [](FlMockBinaryMessenger* messenger, FlValue* event, gpointer user_data) {
        int* event_count = static_cast<int*>(user_data);

        EXPECT_EQ(fl_value_get_type(event), FL_VALUE_TYPE_INT);
        EXPECT_EQ(fl_value_get_int(event), *event_count);

        (*event_count)++;
      },
      [](FlMockBinaryMessenger* messenger, const gchar* code,
         const gchar* message, FlValue* details, gpointer user_data) {},
      &event_count);

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  FlEventChannel* channel1 = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(
      channel1,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        // Send some events.
        for (int i = 0; i < 5; i++) {
          g_autoptr(FlValue) event = fl_value_new_int(100 + i);
          g_autoptr(GError) error = nullptr;
          EXPECT_TRUE(fl_event_channel_send(channel, event, nullptr, &error));
          EXPECT_EQ(error, nullptr);
        }

        return static_cast<FlMethodErrorResponse*>(nullptr);
      },
      nullptr, nullptr, nullptr);

  // Remove this channel
  g_object_unref(channel1);

  // Register a second channel with the same name.
  g_autoptr(FlEventChannel) channel2 = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(
      channel2,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        // Send some events.
        for (int i = 0; i < 5; i++) {
          g_autoptr(FlValue) event = fl_value_new_int(i);
          g_autoptr(GError) error = nullptr;
          EXPECT_TRUE(fl_event_channel_send(channel, event, nullptr, &error));
          EXPECT_EQ(error, nullptr);
        }

        return static_cast<FlMethodErrorResponse*>(nullptr);
      },
      nullptr, nullptr, nullptr);

  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "listen", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);
  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "cancel", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);

  EXPECT_EQ(event_count, 5);
}

// Check can register an event channel replacing an existing one.
TEST(FlEventChannelTest, ReplaceChannel) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  int event_count = 0;
  fl_mock_binary_messenger_set_standard_event_channel(
      messenger, "test",
      [](FlMockBinaryMessenger* messenger, FlValue* event, gpointer user_data) {
        int* event_count = static_cast<int*>(user_data);

        EXPECT_EQ(fl_value_get_type(event), FL_VALUE_TYPE_INT);
        EXPECT_EQ(fl_value_get_int(event), *event_count);

        (*event_count)++;
      },
      [](FlMockBinaryMessenger* messenger, const gchar* code,
         const gchar* message, FlValue* details, gpointer user_data) {},
      &event_count);

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  FlEventChannel* channel1 = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(
      channel1,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        // Send some events.
        for (int i = 0; i < 5; i++) {
          g_autoptr(FlValue) event = fl_value_new_int(100 + i);
          g_autoptr(GError) error = nullptr;
          EXPECT_TRUE(fl_event_channel_send(channel, event, nullptr, &error));
          EXPECT_EQ(error, nullptr);
        }

        return static_cast<FlMethodErrorResponse*>(nullptr);
      },
      nullptr, nullptr, nullptr);

  // Register a second channel with the same name.
  g_autoptr(FlEventChannel) channel2 = fl_event_channel_new(
      FL_BINARY_MESSENGER(messenger), "test", FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(
      channel2,
      [](FlEventChannel* channel, FlValue* args, gpointer user_data) {
        // Send some events.
        for (int i = 0; i < 5; i++) {
          g_autoptr(FlValue) event = fl_value_new_int(i);
          g_autoptr(GError) error = nullptr;
          EXPECT_TRUE(fl_event_channel_send(channel, event, nullptr, &error));
          EXPECT_EQ(error, nullptr);
        }

        return static_cast<FlMethodErrorResponse*>(nullptr);
      },
      nullptr, nullptr, nullptr);

  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "listen", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);
  fl_mock_binary_messenger_invoke_standard_method(
      messenger, "test", "cancel", nullptr,
      [](FlMockBinaryMessenger* messenger, FlMethodResponse* response,
         gpointer user_data) {},
      nullptr);

  EXPECT_EQ(event_count, 5);
}
