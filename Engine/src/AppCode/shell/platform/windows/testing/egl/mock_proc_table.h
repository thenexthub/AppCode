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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_EGL_MOCK_PROC_TABLE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_EGL_MOCK_PROC_TABLE_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/egl/proc_table.h"
#include "gmock/gmock.h"

namespace appcode {
namespace testing {
namespace egl {

/// Mock for the |ProcTable| base class.
class MockProcTable : public ::appcode::egl::ProcTable {
 public:
  MockProcTable() = default;
  virtual ~MockProcTable() = default;

  MOCK_METHOD(void,
              GenTextures,
              (GLsizei n, GLuint* textures),
              (const override));
  MOCK_METHOD(void,
              DeleteTextures,
              (GLsizei n, const GLuint* textures),
              (const override));
  MOCK_METHOD(void,
              BindTexture,
              (GLenum target, GLuint texture),
              (const override));
  MOCK_METHOD(void,
              TexParameteri,
              (GLenum target, GLenum pname, GLint param),
              (const override));
  MOCK_METHOD(void,
              TexImage2D,
              (GLenum target,
               GLint level,
               GLint internalformat,
               GLsizei width,
               GLsizei height,
               GLint border,
               GLenum format,
               GLenum type,
               const void* data),
              (const override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockProcTable);
};

}  // namespace egl
}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_EGL_MOCK_PROC_TABLE_H_
