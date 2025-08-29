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

#include "appcode/shell/platform/common/client_wrapper/include/appcode/method_result_functions.h"

#include <functional>
#include <string>

#include "gtest/gtest.h"

namespace appcode {

// Tests that unset handlers don't cause crashes.
TEST(MethodChannelTest, NoHandlers) {
  MethodResultFunctions<int> result(nullptr, nullptr, nullptr);
  result.Success();
  result.Error("error");
  result.NotImplemented();
}

// Tests that Success calls through to handler.
TEST(MethodChannelTest, Success) {
  bool called = false;
  int value = 1;
  MethodResultFunctions<int> result(
      [&called, value](const int* i) {
        called = true;
        EXPECT_EQ(*i, value);
      },
      nullptr, nullptr);
  result.Success(value);
  EXPECT_TRUE(called);
}

// Tests that Error calls through to handler.
TEST(MethodChannelTest, Error) {
  bool called = false;
  std::string error_code = "a";
  std::string error_message = "b";
  int error_details = 1;
  MethodResultFunctions<int> result(
      nullptr,
      [&called, error_code, error_message, error_details](
          const std::string& code, const std::string& message,
          const int* details) {
        called = true;
        EXPECT_EQ(code, error_code);
        EXPECT_EQ(message, error_message);
        EXPECT_EQ(*details, error_details);
      },
      nullptr);
  result.Error(error_code, error_message, error_details);
  EXPECT_TRUE(called);
}

// Tests that NotImplemented calls through to handler.
TEST(MethodChannelTest, NotImplemented) {
  bool called = false;
  MethodResultFunctions<int> result(nullptr, nullptr,
                                    [&called]() { called = true; });
  result.NotImplemented();
  EXPECT_TRUE(called);
}

}  // namespace appcode
