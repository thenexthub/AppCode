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

#include "filesystem/directory.h"
#include "filesystem/path.h"
#include "filesystem/scoped_temp_dir.h"
#include "gtest/gtest.h"

namespace filesystem {

TEST(Directory, CreateDirectory) {
  std::string cwd = GetCurrentDirectory();

  ScopedTempDir dir;
  EXPECT_TRUE(IsDirectory(dir.path()));
  EXPECT_EQ(0, chdir(dir.path().c_str()));

  EXPECT_TRUE(CreateDirectory("foo/bar"));
  EXPECT_TRUE(IsDirectory("foo"));
  EXPECT_TRUE(IsDirectory("foo/bar"));
  EXPECT_FALSE(IsDirectory("foo/bar/baz"));

  EXPECT_TRUE(CreateDirectory("foo/bar/baz"));
  EXPECT_TRUE(IsDirectory("foo/bar/baz"));

  EXPECT_TRUE(CreateDirectory("qux"));
  EXPECT_TRUE(IsDirectory("qux"));

  EXPECT_EQ(0, chdir(cwd.c_str()));

  std::string abs_path = dir.path() + "/another/one";
  EXPECT_TRUE(CreateDirectory(abs_path));
  EXPECT_TRUE(IsDirectory(abs_path));
}

}  // namespace filesystem
