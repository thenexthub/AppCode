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

#include <unordered_map>

#include "appcode/fml/container.h"

#include "gtest/gtest.h"

namespace fml {
namespace {

TEST(ContainerTest, MapEraseIf) {
  std::unordered_map<int, int> map = {{0, 1}, {2, 3}, {4, 5}};

  fml::erase_if(map, [](std::unordered_map<int, int>::iterator it) {
    return it->first == 0 || it->second == 5;
  });

  EXPECT_EQ(map.size(), 1u);
  EXPECT_TRUE(map.find(2) != map.end());
}

}  // namespace
}  // namespace fml
