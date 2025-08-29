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

#include "appcode/shell/platform/windows/testing/windows_test.h"

#include <string>

#include "appcode/shell/platform/windows/testing/windows_test_context.h"
#include "appcode/testing/testing.h"

namespace appcode {
namespace testing {

WindowsTest::WindowsTest() : context_(GetFixturesDirectory()) {}

std::string WindowsTest::GetFixturesDirectory() const {
  return GetFixturesPath();
}

WindowsTestContext& WindowsTest::GetContext() {
  return context_;
}

}  // namespace testing
}  // namespace appcode
