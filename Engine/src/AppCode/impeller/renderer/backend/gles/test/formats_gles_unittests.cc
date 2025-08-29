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
#include "impeller/renderer/backend/gles/formats_gles.h"

namespace impeller {
namespace testing {

TEST(FormatsGLES, CanFormatFramebufferErrorMessage) {
  ASSERT_EQ(DebugToFramebufferError(GL_FRAMEBUFFER_UNDEFINED),
            "GL_FRAMEBUFFER_UNDEFINED");
  ASSERT_EQ(DebugToFramebufferError(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT),
            "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
  ASSERT_EQ(
      DebugToFramebufferError(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT),
      "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
  ASSERT_EQ(DebugToFramebufferError(GL_FRAMEBUFFER_UNSUPPORTED),
            "GL_FRAMEBUFFER_UNSUPPORTED");
  ASSERT_EQ(DebugToFramebufferError(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE),
            "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
  ASSERT_EQ(DebugToFramebufferError(0), "Unknown error code: 0");
}

}  // namespace testing
}  // namespace impeller
