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

#include "fml/closure.h"
#include "gtest/gtest.h"

TEST(ScopedCleanupClosureTest, DestructorDoesNothingWhenNoClosureSet) {
  fml::ScopedCleanupClosure cleanup;

  // Nothing should happen.
}

TEST(ScopedCleanupClosureTest, ReleaseDoesNothingWhenNoClosureSet) {
  fml::ScopedCleanupClosure cleanup;

  // Nothing should happen.
  EXPECT_EQ(nullptr, cleanup.Release());
}

TEST(ScopedCleanupClosureTest, ClosureInvokedOnDestructorWhenSetInConstructor) {
  auto invoked = false;

  {
    fml::ScopedCleanupClosure cleanup([&invoked]() { invoked = true; });

    EXPECT_FALSE(invoked);
  }

  EXPECT_TRUE(invoked);
}

TEST(ScopedCleanupClosureTest, ClosureInvokedOnDestructorWhenSet) {
  auto invoked = false;

  {
    fml::ScopedCleanupClosure cleanup;
    cleanup.SetClosure([&invoked]() { invoked = true; });

    EXPECT_FALSE(invoked);
  }

  EXPECT_TRUE(invoked);
}

TEST(ScopedCleanupClosureTest, ClosureNotInvokedWhenMoved) {
  auto invoked = 0;

  {
    fml::ScopedCleanupClosure cleanup([&invoked]() { invoked++; });
    fml::ScopedCleanupClosure cleanup2(std::move(cleanup));

    EXPECT_EQ(0, invoked);
  }

  EXPECT_EQ(1, invoked);
}

TEST(ScopedCleanupClosureTest, ClosureNotInvokedWhenMovedViaAssignment) {
  auto invoked = 0;

  {
    fml::ScopedCleanupClosure cleanup([&invoked]() { invoked++; });
    fml::ScopedCleanupClosure cleanup2;
    cleanup2 = std::move(cleanup);

    EXPECT_EQ(0, invoked);
  }

  EXPECT_EQ(1, invoked);
}
