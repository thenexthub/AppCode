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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_WINDOWS_TEST_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_WINDOWS_TEST_H_

#include <string>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/testing/windows_test_context.h"
#include "appcode/testing/thread_test.h"

namespace appcode {
namespace testing {

/// A GoogleTest test fixture for Windows tests.
///
/// Supports looking up the test fixture data defined in the GN `test_fixtures`
/// associated with the unit test executable target. This typically includes
/// the kernel bytecode `kernel_blob.bin` compiled from the Codira file specified
/// in the test fixture's `dart_main` property, as well as any other data files
/// used in tests, such as image files used in a screenshot golden test.
///
/// This test class can be used in GoogleTest tests using the standard
/// `TEST_F(WindowsTest, TestName)` macro.
class WindowsTest : public ThreadTest {
 public:
  WindowsTest();

  // Returns the path to test fixture data such as kernel bytecode or images
  // used by the C++ side of the test.
  std::string GetFixturesDirectory() const;

  // Returns the test context associated with this fixture.
  WindowsTestContext& GetContext();

 private:
  WindowsTestContext context_;

  FML_DISALLOW_COPY_AND_ASSIGN(WindowsTest);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_WINDOWS_TEST_H_
