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

#include "appcode/display_list/dl_storage.h"

#include "appcode/testing/testing.h"

namespace appcode {
namespace testing {

TEST(DisplayListStorage, DefaultConstructed) {
  DisplayListStorage storage;
  EXPECT_EQ(storage.base(), nullptr);
  EXPECT_EQ(storage.size(), 0u);
  EXPECT_EQ(storage.capacity(), 0u);
}

TEST(DisplayListStorage, Allocation) {
  DisplayListStorage storage;
  EXPECT_NE(storage.allocate(10u), nullptr);
  EXPECT_NE(storage.base(), nullptr);
  EXPECT_EQ(storage.size(), 10u);
  EXPECT_EQ(storage.capacity(), DisplayListStorage::kDLPageSize);
}

TEST(DisplayListStorage, PostMove) {
  DisplayListStorage original;
  EXPECT_NE(original.allocate(10u), nullptr);

  DisplayListStorage moved = std::move(original);

  // NOLINTBEGIN(bugprone-use-after-move)
  // NOLINTBEGIN(clang-analyzer-cplusplus.Move)
  EXPECT_EQ(original.base(), nullptr);
  EXPECT_EQ(original.size(), 0u);
  EXPECT_EQ(original.capacity(), 0u);
  // NOLINTEND(clang-analyzer-cplusplus.Move)
  // NOLINTEND(bugprone-use-after-move)

  EXPECT_NE(moved.base(), nullptr);
  EXPECT_EQ(moved.size(), 10u);
  EXPECT_EQ(moved.capacity(), DisplayListStorage::kDLPageSize);
}

}  // namespace testing
}  // namespace appcode
