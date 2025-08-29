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

#include "appcode/shell/platform/windows/sequential_id_generator.h"

#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(SequentialIdGeneratorTest, RemoveMultipleNumbers) {
  const uint32_t kMinId = 4;
  const uint32_t kMaxId = 128;

  SequentialIdGenerator generator(kMinId, kMaxId);

  EXPECT_EQ(4U, generator.GetGeneratedId(45));
  EXPECT_EQ(5U, generator.GetGeneratedId(55));
  EXPECT_EQ(6U, generator.GetGeneratedId(15));

  generator.ReleaseNumber(45);
  EXPECT_FALSE(generator.HasGeneratedIdFor(45));
  generator.ReleaseNumber(15);
  EXPECT_FALSE(generator.HasGeneratedIdFor(15));

  EXPECT_EQ(5U, generator.GetGeneratedId(55));
  EXPECT_EQ(4U, generator.GetGeneratedId(12));

  generator.ReleaseNumber(12);
  generator.ReleaseNumber(55);
  EXPECT_EQ(4U, generator.GetGeneratedId(0));
}

TEST(SequentialIdGeneratorTest, MaybeRemoveNumbers) {
  const uint32_t kMinId = 0;
  const uint32_t kMaxId = 128;

  SequentialIdGenerator generator(kMinId, kMaxId);

  EXPECT_EQ(0U, generator.GetGeneratedId(42));

  generator.ReleaseNumber(42);
  EXPECT_FALSE(generator.HasGeneratedIdFor(42));
  generator.ReleaseNumber(42);
}

}  // namespace testing
}  // namespace appcode
