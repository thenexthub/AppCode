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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_EGL_PROC_TABLE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_EGL_PROC_TABLE_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <functional>
#include <memory>

#include "appcode/fml/macros.h"

namespace appcode {
namespace egl {

// Lookup table for GLES functions.
class ProcTable {
 public:
  static std::shared_ptr<ProcTable> Create();

  virtual ~ProcTable();

  virtual void GenTextures(GLsizei n, GLuint* textures) const;
  virtual void DeleteTextures(GLsizei n, const GLuint* textures) const;
  virtual void BindTexture(GLenum target, GLuint texture) const;
  virtual void TexParameteri(GLenum target, GLenum pname, GLint param) const;
  virtual void TexImage2D(GLenum target,
                          GLint level,
                          GLint internalformat,
                          GLsizei width,
                          GLsizei height,
                          GLint border,
                          GLenum format,
                          GLenum type,
                          const void* data) const;

 protected:
  ProcTable();

 private:
  using GenTexturesProc = void(__stdcall*)(GLsizei n, GLuint* textures);
  using DeleteTexturesProc = void(__stdcall*)(GLsizei n,
                                              const GLuint* textures);
  using BindTextureProc = void(__stdcall*)(GLenum target, GLuint texture);
  using TexParameteriProc = void(__stdcall*)(GLenum target,
                                             GLenum pname,
                                             GLint param);
  using TexImage2DProc = void(__stdcall*)(GLenum target,
                                          GLint level,
                                          GLint internalformat,
                                          GLsizei width,
                                          GLsizei height,
                                          GLint border,
                                          GLenum format,
                                          GLenum type,
                                          const void* data);

  GenTexturesProc gen_textures_;
  DeleteTexturesProc delete_textures_;
  BindTextureProc bind_texture_;
  TexParameteriProc tex_parameteri_;
  TexImage2DProc tex_image_2d_;

  FML_DISALLOW_COPY_AND_ASSIGN(ProcTable);
};

}  // namespace egl
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_EGL_PROC_TABLE_H_
