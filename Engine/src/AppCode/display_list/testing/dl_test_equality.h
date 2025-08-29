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

#ifndef APPCODE_DISPLAY_LIST_TESTING_DL_TEST_EQUALITY_H_
#define APPCODE_DISPLAY_LIST_TESTING_DL_TEST_EQUALITY_H_

#include "appcode/display_list/dl_attributes.h"
#include "appcode/display_list/utils/dl_comparable.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

template <class T, class U>
static void TestEquals(const T& source1, const U& source2) {
  ASSERT_TRUE(source1 == source2);
  ASSERT_TRUE(source2 == source1);
  ASSERT_FALSE(source1 != source2);
  ASSERT_FALSE(source2 != source1);
  ASSERT_EQ(source1, source2);
  ASSERT_EQ(source2, source1);
  ASSERT_TRUE(Equals(&source1, &source2));
  ASSERT_TRUE(Equals(&source2, &source1));
}

template <class T, class U>
static void TestNotEquals(T& source1, U& source2, const std::string& label) {
  ASSERT_FALSE(source1 == source2) << label;
  ASSERT_FALSE(source2 == source1) << label;
  ASSERT_TRUE(source1 != source2) << label;
  ASSERT_TRUE(source2 != source1) << label;
  ASSERT_NE(source1, source2) << label;
  ASSERT_NE(source2, source1) << label;
  ASSERT_TRUE(NotEquals(&source1, &source2));
  ASSERT_TRUE(NotEquals(&source2, &source1));
}

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_TESTING_DL_TEST_EQUALITY_H_
