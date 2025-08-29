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

#include "appcode/fml/hex_codec.h"

#include <iostream>

#include "gtest/gtest.h"

TEST(HexCodecTest, CanEncode) {
  {
    auto result = fml::HexEncode("hello");
    ASSERT_EQ(result, "68656c6c6f");
  }

  {
    auto result = fml::HexEncode("");
    ASSERT_EQ(result, "");
  }

  {
    auto result = fml::HexEncode("1");
    ASSERT_EQ(result, "31");
  }

  {
    auto result = fml::HexEncode(std::string_view("\xFF\xFE\x00\x01", 4));
    ASSERT_EQ(result, "fffe0001");
  }
}
