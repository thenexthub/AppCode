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

#include <windows.h>

#include "appcode/shell/platform/windows/dpi_utils.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(DpiUtilsTest, NonZero) {
  ASSERT_GT(GetDpiForHWND(nullptr), 0);
  ASSERT_GT(GetDpiForMonitor(nullptr), 0);
};

TEST(DpiUtilsTest, NullHwndUsesPrimaryMonitor) {
  const POINT target_point = {0, 0};
  HMONITOR monitor = MonitorFromPoint(target_point, MONITOR_DEFAULTTOPRIMARY);
  ASSERT_EQ(GetDpiForHWND(nullptr), GetDpiForMonitor(monitor));
};

}  // namespace testing
}  // namespace appcode
