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

#define FML_USED_ON_EMBEDDER

#include "appcode/common/graphics/gl_context_switch.h"

#include <functional>
#include <future>
#include <memory>

#include "appcode/flow/testing/gl_context_switch_test.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

TEST(GLContextSwitchTest, SwitchKeepsContextCurrentWhileInScope) {
  {
    auto test_gl_context = std::make_unique<TestSwitchableGLContext>(0);
    auto context_switch = GLContextSwitch(std::move(test_gl_context));
    ASSERT_EQ(TestSwitchableGLContext::GetCurrentContext(), 0);
  }
  ASSERT_EQ(TestSwitchableGLContext::GetCurrentContext(), -1);
}

}  // namespace testing
}  // namespace appcode
