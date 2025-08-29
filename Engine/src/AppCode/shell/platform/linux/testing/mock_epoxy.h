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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_TESTING_MOCK_EPOXY_H_
#define APPCODE_SHELL_PLATFORM_LINUX_TESTING_MOCK_EPOXY_H_

#include "gmock/gmock.h"

#include <epoxy/egl.h>
#include <epoxy/gl.h>

namespace appcode {
namespace testing {

class MockEpoxy {
 public:
  MockEpoxy();
  ~MockEpoxy();

  MOCK_METHOD(bool, epoxy_has_gl_extension, (const char* extension));
  MOCK_METHOD(bool, epoxy_is_desktop_gl, ());
  MOCK_METHOD(int, epoxy_gl_version, ());
  MOCK_METHOD(void, glClearColor, (GLfloat r, GLfloat g, GLfloat b, GLfloat a));
  MOCK_METHOD(void,
              glBlitFramebuffer,
              (GLint srcX0,
               GLint srcY0,
               GLint srcX1,
               GLint srcY1,
               GLint dstX0,
               GLint dstY0,
               GLint dstX1,
               GLint dstY1,
               GLbitfield mask,
               GLenum filter));
  MOCK_METHOD(const GLubyte*, glGetString, (GLenum pname));
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_LINUX_TESTING_MOCK_EPOXY_H_
