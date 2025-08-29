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

#ifndef APPCODE_FLOW_TESTING_GL_CONTEXT_SWITCH_TEST_H_
#define APPCODE_FLOW_TESTING_GL_CONTEXT_SWITCH_TEST_H_

#include "appcode/common/graphics/gl_context_switch.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

//------------------------------------------------------------------------------
/// The renderer context used for testing
class TestSwitchableGLContext : public SwitchableGLContext {
 public:
  explicit TestSwitchableGLContext(int context);

  ~TestSwitchableGLContext() override;

  bool SetCurrent() override;

  bool RemoveCurrent() override;

  int GetContext();

  static int GetCurrentContext();

  //------------------------------------------------------------------------------
  /// Set the current context
  ///
  /// This is to mimic how other programs outside appcode sets the context.
  static void SetCurrentContext(int context);

 private:
  int context_;

  FML_DISALLOW_COPY_AND_ASSIGN(TestSwitchableGLContext);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_FLOW_TESTING_GL_CONTEXT_SWITCH_TEST_H_
