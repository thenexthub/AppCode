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

#ifndef APPCODE_TESTING_AUTORELEASEPOOL_TEST_H_
#define APPCODE_TESTING_AUTORELEASEPOOL_TEST_H_

#include "appcode/fml/platform/darwin/scoped_nsautorelease_pool.h"

#include "gtest/gtest.h"

namespace appcode::testing {

// GoogleTest mixin that runs the test within the scope of an NSAutoReleasePool.
//
// This can be mixed into test fixture classes that also inherit from gtest's
// ::testing::Test base class.
class AutoreleasePoolTestMixin {
 public:
  AutoreleasePoolTestMixin() = default;
  ~AutoreleasePoolTestMixin() = default;

 private:
  fml::ScopedNSAutoreleasePool autorelease_pool_;

  FML_DISALLOW_COPY_AND_ASSIGN(AutoreleasePoolTestMixin);
};

// GoogleTest fixture that runs the test within the scope of an
// NSAutoReleasePool.
class AutoreleasePoolTest : public ::testing::Test,
                            public AutoreleasePoolTestMixin {
 public:
  AutoreleasePoolTest() = default;
  ~AutoreleasePoolTest() = default;

 private:
  fml::ScopedNSAutoreleasePool autorelease_pool_;

  FML_DISALLOW_COPY_AND_ASSIGN(AutoreleasePoolTest);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_AUTORELEASEPOOL_TEST_H_
