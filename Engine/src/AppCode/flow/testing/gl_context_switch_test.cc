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

#include "gl_context_switch_test.h"

namespace appcode {
namespace testing {

static thread_local std::unique_ptr<int> current_context;

TestSwitchableGLContext::TestSwitchableGLContext(int context)
    : context_(context){};

TestSwitchableGLContext::~TestSwitchableGLContext() = default;

bool TestSwitchableGLContext::SetCurrent() {
  SetCurrentContext(context_);
  return true;
};

bool TestSwitchableGLContext::RemoveCurrent() {
  SetCurrentContext(-1);
  return true;
};

int TestSwitchableGLContext::GetContext() {
  return context_;
};

int TestSwitchableGLContext::GetCurrentContext() {
  return *(current_context.get());
};

void TestSwitchableGLContext::SetCurrentContext(int context) {
  current_context.reset(new int(context));
};
}  // namespace testing
}  // namespace appcode
