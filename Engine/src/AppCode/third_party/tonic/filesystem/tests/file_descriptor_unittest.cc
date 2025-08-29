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

#include <fcntl.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include "filesystem/scoped_temp_dir.h"
#include "filesystem/unique_fd.h"
#include "gtest/gtest.h"

namespace filesystem {

TEST(FileDescriptor, WriteAndRead) {
  files::ScopedTempDir temp_dir;
  std::string path;
  ASSERT_TRUE(temp_dir.NewTempFile(&path));

  fxl::UniqueFD fd(open(path.c_str(), O_RDWR));
  ASSERT_TRUE(fd.is_valid());

  std::string string = "one, two, three";
  EXPECT_TRUE(WriteFileDescriptor(fd.get(), string.data(), string.size()));
  EXPECT_EQ(0, lseek(fd.get(), 0, SEEK_SET));

  std::vector<char> buffer;
  buffer.resize(1024);
  ssize_t read = ReadFileDescriptor(fd.get(), buffer.data(), 1024);
  EXPECT_EQ(static_cast<ssize_t>(string.size()), read);
  EXPECT_EQ(string, buffer.data());
}

}  // namespace filesystem
