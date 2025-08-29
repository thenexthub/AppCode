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

#include <windows.h>

#include "appcode/shell/platform/windows/testing/test_keyboard.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(TestKeyboard, CloneString) {
  const char* str1 = "123";
  char* cloned_str1 = clone_string(str1);
  EXPECT_STREQ(str1, cloned_str1);
  EXPECT_NE(str1, cloned_str1);
  delete[] cloned_str1;

  EXPECT_EQ(clone_string(nullptr), nullptr);
};

TEST(TestKeyboard, CreateKeyEventLparam) {
  EXPECT_EQ(CreateKeyEventLparam(0x1, true, true), 0xC1010001);

  EXPECT_EQ(CreateKeyEventLparam(0x05, false, false, 0, 1, 0), 0x20050000);
};

}  // namespace testing
}  // namespace appcode
