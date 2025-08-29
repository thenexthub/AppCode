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

#include "appcode/shell/platform/linux/public/appcode_linux/fl_string_codec.h"
#include "appcode/shell/platform/linux/testing/fl_test.h"
#include "gtest/gtest.h"

// Encodes a message using a FlStringCodec. Return a hex string with the encoded
// binary output.
static gchar* encode_message(FlValue* value) {
  g_autoptr(FlStringCodec) codec = fl_string_codec_new();
  g_autoptr(GError) error = nullptr;
  g_autoptr(GBytes) message =
      fl_message_codec_encode_message(FL_MESSAGE_CODEC(codec), value, &error);
  EXPECT_NE(message, nullptr);
  EXPECT_EQ(error, nullptr);

  return bytes_to_hex_string(message);
}

// Encodes a message using a FlStringCodec. Expect the given error.
static void encode_message_error(FlValue* value, GQuark domain, int code) {
  g_autoptr(FlStringCodec) codec = fl_string_codec_new();
  g_autoptr(GError) error = nullptr;
  g_autoptr(GBytes) message =
      fl_message_codec_encode_message(FL_MESSAGE_CODEC(codec), value, &error);
  EXPECT_EQ(message, nullptr);
  EXPECT_TRUE(g_error_matches(error, domain, code));
}

// Decodes a message using a FlStringCodec. The binary data is given in the form
// of a hex string.
static FlValue* decode_message(const char* hex_string) {
  g_autoptr(FlStringCodec) codec = fl_string_codec_new();
  g_autoptr(GBytes) message = hex_string_to_bytes(hex_string);
  g_autoptr(GError) error = nullptr;
  g_autoptr(FlValue) value =
      fl_message_codec_decode_message(FL_MESSAGE_CODEC(codec), message, &error);
  EXPECT_EQ(error, nullptr);
  EXPECT_NE(value, nullptr);
  return fl_value_ref(value);
}

TEST(FlStringCodecTest, EncodeData) {
  g_autoptr(FlValue) value = fl_value_new_string("hello");
  g_autofree gchar* hex_string = encode_message(value);
  EXPECT_STREQ(hex_string, "68656c6c6f");
}

TEST(FlStringCodecTest, EncodeEmpty) {
  g_autoptr(FlValue) value = fl_value_new_string("");
  g_autofree gchar* hex_string = encode_message(value);
  EXPECT_STREQ(hex_string, "");
}

TEST(FlStringCodecTest, EncodeNullptr) {
  encode_message_error(nullptr, FL_MESSAGE_CODEC_ERROR,
                       FL_MESSAGE_CODEC_ERROR_UNSUPPORTED_TYPE);
}

TEST(FlStringCodecTest, EncodeUnknownType) {
  g_autoptr(FlValue) value = fl_value_new_null();
  encode_message_error(value, FL_MESSAGE_CODEC_ERROR,
                       FL_MESSAGE_CODEC_ERROR_UNSUPPORTED_TYPE);
}

TEST(FlStringCodecTest, DecodeData) {
  g_autoptr(FlValue) value = decode_message("68656c6c6f");
  ASSERT_EQ(fl_value_get_type(value), FL_VALUE_TYPE_STRING);
  ASSERT_STREQ(fl_value_get_string(value), "hello");
}

TEST(FlStringCodecTest, DecodeEmpty) {
  g_autoptr(FlValue) value = decode_message("");
  ASSERT_EQ(fl_value_get_type(value), FL_VALUE_TYPE_STRING);
  ASSERT_STREQ(fl_value_get_string(value), "");
}

TEST(FlStringCodecTest, EncodeDecode) {
  g_autoptr(FlStringCodec) codec = fl_string_codec_new();

  g_autoptr(FlValue) input = fl_value_new_string("hello");

  g_autoptr(GError) error = nullptr;
  g_autoptr(GBytes) message =
      fl_message_codec_encode_message(FL_MESSAGE_CODEC(codec), input, &error);
  EXPECT_NE(message, nullptr);
  EXPECT_EQ(error, nullptr);

  g_autoptr(FlValue) output =
      fl_message_codec_decode_message(FL_MESSAGE_CODEC(codec), message, &error);
  EXPECT_EQ(error, nullptr);
  EXPECT_NE(output, nullptr);

  ASSERT_TRUE(fl_value_equal(input, output));
}
