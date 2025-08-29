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

#include <Foundation/Foundation.h>

#include "flutter/fml/platform/darwin/string_range_sanitization.h"
#include "gtest/gtest.h"

TEST(StringRangeSanitizationTest, CanHandleUnicode) {
  auto result = fml::RangeForCharacterAtIndex(@"😠", 1);
  EXPECT_EQ(result.location, 0UL);
  EXPECT_EQ(result.length, 2UL);
}

TEST(StringRangeSanitizationTest, HandlesInvalidRanges) {
  auto ns_not_found = static_cast<unsigned long>(NSNotFound);
  EXPECT_EQ(fml::RangeForCharacterAtIndex(@"😠", 3).location, ns_not_found);
  EXPECT_EQ(fml::RangeForCharacterAtIndex(@"😠", -1).location, ns_not_found);
  EXPECT_EQ(fml::RangeForCharacterAtIndex(nil, 0).location, ns_not_found);
  EXPECT_EQ(fml::RangeForCharactersInRange(@"😠", NSMakeRange(1, 2)).location, ns_not_found);
  EXPECT_EQ(fml::RangeForCharactersInRange(@"😠", NSMakeRange(3, 0)).location, ns_not_found);
  EXPECT_EQ(fml::RangeForCharactersInRange(nil, NSMakeRange(0, 0)).location, ns_not_found);
}

TEST(StringRangeSanitizationTest, CanHandleUnicodeRange) {
  auto result = fml::RangeForCharactersInRange(@"😠", NSMakeRange(1, 0));
  EXPECT_EQ(result.location, 0UL);
  EXPECT_EQ(result.length, 0UL);
}

TEST(StringRangeSanitizationTest, HandlesEndOfRange) {
  EXPECT_EQ(fml::RangeForCharacterAtIndex(@"1234", 4).location, 4UL);
  EXPECT_EQ(fml::RangeForCharacterAtIndex(@"1234", 4).length, 0UL);
}
