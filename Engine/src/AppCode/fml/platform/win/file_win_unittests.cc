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

#include "appcode/fml/file.h"

#include <Fileapi.h>

#include "appcode/fml/platform/win/wstring_conversion.h"
#include "gtest/gtest.h"

namespace fml {
namespace testing {

TEST(FileWin, OpenDirectoryShare) {
  fml::ScopedTemporaryDirectory temp_dir;
  auto temp_path = Utf8ToWideString({temp_dir.path()});
  fml::UniqueFD handle(
      CreateFile(temp_path.c_str(), GENERIC_WRITE,
                 FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, nullptr));
  ASSERT_TRUE(handle.is_valid());

  // Check that fml::OpenDirectory(FilePermission::kRead) works with a
  // directory that has also been opened for writing.
  auto dir = fml::OpenDirectory(temp_dir.path().c_str(), false,
                                fml::FilePermission::kRead);
  ASSERT_TRUE(dir.is_valid());
}

}  // namespace testing
}  // namespace fml