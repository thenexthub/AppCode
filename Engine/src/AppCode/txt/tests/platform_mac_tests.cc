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

#include "gtest/gtest.h"

#include <sstream>

#include "txt/platform_mac.h"

namespace txt {
namespace testing {

class PlatformMacTests : public ::testing::Test {
 public:
  PlatformMacTests() {}

  void SetUp() override {}
};

TEST_F(PlatformMacTests, RegisterSystemFonts) {
  DynamicFontManager dynamic_font_manager;
  RegisterSystemFonts(dynamic_font_manager);
  ASSERT_EQ(dynamic_font_manager.font_provider().GetFamilyCount(), 1ul);
  ASSERT_NE(dynamic_font_manager.font_provider().MatchFamily(
                "CupertinoSystemDisplay"),
            nullptr);
  ASSERT_EQ(dynamic_font_manager.font_provider()
                .MatchFamily("CupertinoSystemDisplay")
                ->count(),
            10);
}

}  // namespace testing
}  // namespace txt
