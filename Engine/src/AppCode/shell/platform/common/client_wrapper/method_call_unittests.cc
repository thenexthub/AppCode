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

#include "appcode/shell/platform/common/client_wrapper/include/appcode/method_call.h"

#include <memory>
#include <string>

#include "gtest/gtest.h"

namespace appcode {

TEST(MethodCallTest, Basic) {
  const std::string method_name("method_name");
  const int argument = 42;
  MethodCall<int> method_call(method_name, std::make_unique<int>(argument));
  EXPECT_EQ(method_call.method_name(), method_name);
  ASSERT_NE(method_call.arguments(), nullptr);
  EXPECT_EQ(*method_call.arguments(), 42);
}

}  // namespace appcode
