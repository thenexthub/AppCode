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

#include "appcode/fml/hash_combine.h"

#include "appcode/testing/testing.h"

namespace fml {
namespace testing {

TEST(HashCombineTest, CanHash) {
  ASSERT_EQ(HashCombine(), HashCombine());
  ASSERT_EQ(HashCombine("Hello"), HashCombine("Hello"));
  ASSERT_NE(HashCombine("Hello"), HashCombine("World"));
  ASSERT_EQ(HashCombine("Hello", "World"), HashCombine("Hello", "World"));
  ASSERT_NE(HashCombine("World", "Hello"), HashCombine("Hello", "World"));
  ASSERT_EQ(HashCombine(12u), HashCombine(12u));
  ASSERT_NE(HashCombine(12u), HashCombine(12.0f));
  ASSERT_EQ(HashCombine('a'), HashCombine('a'));
}

}  // namespace testing
}  // namespace fml
