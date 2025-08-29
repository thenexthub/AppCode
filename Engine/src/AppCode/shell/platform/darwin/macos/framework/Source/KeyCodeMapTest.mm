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

#import "KeyCodeMap_Internal.h"

#import "flutter/testing/testing.h"
#include "third_party/googletest/googletest/include/gtest/gtest.h"

namespace flutter {

bool operator==(const LayoutGoal& a, const LayoutGoal& b) {
  return a.keyCode == b.keyCode && a.keyChar == b.keyChar && a.mandatory == b.mandatory;
}

namespace testing {

// Spot check some expected values so that we know that some classes of key
// aren't excluded.
TEST(KeyMappingTest, HasExpectedValues) {
  // Has Space
  EXPECT_NE(std::find(kLayoutGoals.begin(), kLayoutGoals.end(), LayoutGoal{0x31, 0x20, false}),
            kLayoutGoals.end());
  // Has Digit0
  EXPECT_NE(std::find(kLayoutGoals.begin(), kLayoutGoals.end(), LayoutGoal{0x1d, 0x30, true}),
            kLayoutGoals.end());
  // Has KeyA
  EXPECT_NE(std::find(kLayoutGoals.begin(), kLayoutGoals.end(), LayoutGoal{0x00, 0x61, true}),
            kLayoutGoals.end());
  // Has Equal
  EXPECT_NE(std::find(kLayoutGoals.begin(), kLayoutGoals.end(), LayoutGoal{0x18, 0x3d, false}),
            kLayoutGoals.end());
  // Has IntlBackslash
  EXPECT_NE(
      std::find(kLayoutGoals.begin(), kLayoutGoals.end(), LayoutGoal{0x0a, 0x200000020, false}),
      kLayoutGoals.end());
}
}  // namespace testing
}  // namespace flutter
