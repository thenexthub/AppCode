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

#include "appcode/fml/time/time_delta.h"

#include "gtest/gtest.h"

namespace fml {
namespace {

TEST(TimeDelta, Control) {
  EXPECT_LT(TimeDelta::Min(), TimeDelta::Zero());
  EXPECT_GT(TimeDelta::Max(), TimeDelta::Zero());

  EXPECT_GT(TimeDelta::Zero(), TimeDelta::FromMilliseconds(-100));
  EXPECT_LT(TimeDelta::Zero(), TimeDelta::FromMilliseconds(100));

  EXPECT_EQ(TimeDelta::FromMilliseconds(1000), TimeDelta::FromSeconds(1));
}

}  // namespace
}  // namespace fml
