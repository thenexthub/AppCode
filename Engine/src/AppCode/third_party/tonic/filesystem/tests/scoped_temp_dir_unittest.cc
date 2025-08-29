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

#include "filesystem/scoped_temp_dir.h"
#include "filesystem/directory.h"
#include "filesystem/path.h"
#include "gtest/gtest.h"

namespace filesystem {

TEST(ScopedTempDir, Creation) {
  ScopedTempDir dir;

  EXPECT_TRUE(IsDirectory(dir.path()));
}

TEST(ScopedTempDir, Deletion) {
  std::string path;
  {
    ScopedTempDir dir;
    path = dir.path();
  }

  EXPECT_FALSE(IsDirectory(path));
}

TEST(ScopedTempDir, NewTempFile) {
  ScopedTempDir dir;
  std::string path;
  EXPECT_TRUE(dir.NewTempFile(&path));
  EXPECT_FALSE(path.empty());
}

TEST(ScopedTempDir, CustomParent) {
  ScopedTempDir root_dir;
  std::string parent = root_dir.path() + "/a/b/c";
  std::string path;
  {
    ScopedTempDir dir(parent);
    path = dir.path();
    EXPECT_TRUE(IsDirectory(path));
    EXPECT_EQ(path.substr(0, parent.size()), parent);
    EXPECT_NE("temp_dir_XXXXXX", GetBaseName(path));

    // Regression test - don't create temp_dir_XXXXXX dir next to the temp one.
    EXPECT_FALSE(
        files::IsDirectory(GetDirectoryName(path) + "/temp_dir_XXXXXX"));
  }

  // Verify that the tmp directory itself was deleted, but not the parent.
  EXPECT_FALSE(IsDirectory(path));
  EXPECT_TRUE(IsDirectory(parent));
}

}  // namespace filesystem
