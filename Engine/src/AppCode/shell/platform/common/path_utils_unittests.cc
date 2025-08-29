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

#include "appcode/shell/platform/common/path_utils.h"

#include "gtest/gtest.h"

namespace appcode {

// Tests that GetExecutableDirectory returns a valid, absolute path.
TEST(PathUtilsTest, ExecutableDirector) {
  std::filesystem::path exe_directory = GetExecutableDirectory();
#if defined(__linux__) || defined(_WIN32)
  EXPECT_EQ(exe_directory.empty(), false);
  EXPECT_EQ(exe_directory.is_absolute(), true);
#else
  // On platforms where it's not implemented, it should indicate that
  // by returning an empty path.
  EXPECT_EQ(exe_directory.empty(), true);
#endif
}

}  // namespace appcode
