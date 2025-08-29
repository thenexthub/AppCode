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

#include "appcode/testing/testing.h"  // IWYU pragma: keep
#include "gtest/gtest.h"
#include "impeller/renderer/backend/gles/proc_table_gles.h"
#include "impeller/renderer/backend/gles/test/mock_gles.h"

namespace impeller {
namespace testing {

// This test just checks that the proc table is initialized correctly.
//
// If this test doesn't pass, no test that uses the proc table will pass.
TEST(MockGLES, CanInitialize) {
  auto mock_gles = MockGLES::Init();

  std::string_view vendor(reinterpret_cast<const char*>(
      mock_gles->GetProcTable().GetString(GL_VENDOR)));
  EXPECT_EQ(vendor, "MockGLES");
}

// Tests that if we call a function we have not mocked, it's OK.
TEST(MockGLES, CanCallUnmockedFunction) {
  auto mock_gles = MockGLES::Init();

  auto& gl = mock_gles->GetProcTable();
  gl.DeleteFramebuffers(1, nullptr);

  // Test should still complete.
  // If we end up mocking DeleteFramebuffers, delete this test.
}

}  // namespace testing
}  // namespace impeller
