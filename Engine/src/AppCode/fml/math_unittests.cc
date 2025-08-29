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

#include <cmath>

#include "appcode/fml/math.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(MathTest, Constants) {
  // Don't use the constants in cmath as those aren't portable.
  EXPECT_FLOAT_EQ(std::log2(math::kE), math::kLog2E);
  EXPECT_FLOAT_EQ(std::log10(math::kE), math::kLog10E);
  EXPECT_FLOAT_EQ(std::log(2.0f), math::kLogE2);
  EXPECT_FLOAT_EQ(math::kPi / 2.0f, math::kPiOver2);
  EXPECT_FLOAT_EQ(math::kPi / 4.0f, math::kPiOver4);
  EXPECT_FLOAT_EQ(1.0f / math::kPi, math::k1OverPi);
  EXPECT_FLOAT_EQ(2.0f / math::kPi, math::k2OverPi);
  EXPECT_FLOAT_EQ(std::sqrt(2.0f), math::kSqrt2);
  EXPECT_FLOAT_EQ(1.0f / std::sqrt(2.0f), math::k1OverSqrt2);
}

}  // namespace testing
}  // namespace appcode
