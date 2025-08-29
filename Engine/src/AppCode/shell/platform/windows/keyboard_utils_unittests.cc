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
#include "appcode/shell/platform/windows/keyboard_utils.h"

#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(KeyboardWin32CommonTest, EncodeUtf16) {
  std::u16string result;

  result = EncodeUtf16(0x24);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], 0x24);

  result = EncodeUtf16(0x20AC);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], 0x20AC);

  result = EncodeUtf16(0x10437);
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result[0], 0xD801);
  EXPECT_EQ(result[1], 0xDC37);

  result = EncodeUtf16(0x24B62);
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result[0], 0xD852);
  EXPECT_EQ(result[1], 0xDF62);
}

}  // namespace testing
}  // namespace appcode
