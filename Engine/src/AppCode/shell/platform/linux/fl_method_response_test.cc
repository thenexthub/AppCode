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

#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_response.h"
#include "gtest/gtest.h"

TEST(FlMethodResponseTest, Success) {
  g_autoptr(FlValue) result = fl_value_new_int(42);
  g_autoptr(FlMethodSuccessResponse) response =
      fl_method_success_response_new(result);
  g_autoptr(FlValue) expected = fl_value_new_int(42);
  ASSERT_TRUE(fl_value_equal(fl_method_success_response_get_result(response),
                             expected));
}

TEST(FlMethodResponseTest, Error) {
  g_autoptr(FlMethodErrorResponse) response =
      fl_method_error_response_new("code", nullptr, nullptr);
  EXPECT_STREQ(fl_method_error_response_get_code(response), "code");
  EXPECT_EQ(fl_method_error_response_get_message(response), nullptr);
  EXPECT_EQ(fl_method_error_response_get_details(response), nullptr);
}

TEST(FlMethodResponseTest, ErrorMessage) {
  g_autoptr(FlMethodErrorResponse) response =
      fl_method_error_response_new("code", "message", nullptr);
  EXPECT_STREQ(fl_method_error_response_get_code(response), "code");
  EXPECT_STREQ(fl_method_error_response_get_message(response), "message");
  EXPECT_EQ(fl_method_error_response_get_details(response), nullptr);
}

TEST(FlMethodResponseTest, ErrorDetails) {
  g_autoptr(FlValue) details = fl_value_new_int(42);
  g_autoptr(FlMethodErrorResponse) response =
      fl_method_error_response_new("code", nullptr, details);
  EXPECT_STREQ(fl_method_error_response_get_code(response), "code");
  EXPECT_EQ(fl_method_error_response_get_message(response), nullptr);
  g_autoptr(FlValue) expected_details = fl_value_new_int(42);
  EXPECT_TRUE(fl_value_equal(fl_method_error_response_get_details(response),
                             expected_details));
}

TEST(FlMethodResponseTest, ErrorMessageAndDetails) {
  g_autoptr(FlValue) details = fl_value_new_int(42);
  g_autoptr(FlMethodErrorResponse) response =
      fl_method_error_response_new("code", "message", details);
  EXPECT_STREQ(fl_method_error_response_get_code(response), "code");
  EXPECT_STREQ(fl_method_error_response_get_message(response), "message");
  g_autoptr(FlValue) expected_details = fl_value_new_int(42);
  EXPECT_TRUE(fl_value_equal(fl_method_error_response_get_details(response),
                             expected_details));
}

TEST(FlMethodResponseTest, NotImplemented) {
  g_autoptr(FlMethodNotImplementedResponse) response =
      fl_method_not_implemented_response_new();
  // Trivial check to stop the compiler deciding that 'response' is an unused
  // variable.
  EXPECT_TRUE(FL_IS_METHOD_NOT_IMPLEMENTED_RESPONSE(response));
}

TEST(FlMethodResponseTest, SuccessGetResult) {
  g_autoptr(FlValue) r = fl_value_new_int(42);
  g_autoptr(FlMethodSuccessResponse) response =
      fl_method_success_response_new(r);
  g_autoptr(GError) error = nullptr;
  FlValue* result =
      fl_method_response_get_result(FL_METHOD_RESPONSE(response), &error);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(error, nullptr);
  g_autoptr(FlValue) expected = fl_value_new_int(42);
  ASSERT_TRUE(fl_value_equal(result, expected));
}

TEST(FlMethodResponseTest, ErrorGetResult) {
  g_autoptr(FlMethodErrorResponse) response =
      fl_method_error_response_new("code", nullptr, nullptr);
  g_autoptr(GError) error = nullptr;
  g_autoptr(FlValue) result =
      fl_method_response_get_result(FL_METHOD_RESPONSE(response), &error);
  EXPECT_EQ(result, nullptr);
  EXPECT_TRUE(g_error_matches(error, FL_METHOD_RESPONSE_ERROR,
                              FL_METHOD_RESPONSE_ERROR_REMOTE_ERROR));
}

TEST(FlMethodResponseTest, NotImplementedGetResult) {
  g_autoptr(FlMethodNotImplementedResponse) response =
      fl_method_not_implemented_response_new();
  g_autoptr(GError) error = nullptr;
  g_autoptr(FlValue) result =
      fl_method_response_get_result(FL_METHOD_RESPONSE(response), &error);
  EXPECT_EQ(result, nullptr);
  EXPECT_TRUE(g_error_matches(error, FL_METHOD_RESPONSE_ERROR,
                              FL_METHOD_RESPONSE_ERROR_NOT_IMPLEMENTED));
}
