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

#include "appcode/shell/platform/windows/egl/proc_table.h"

#include <EGL/egl.h>

namespace appcode {
namespace egl {

std::shared_ptr<ProcTable> ProcTable::Create() {
  auto gl = std::shared_ptr<ProcTable>(new ProcTable());

  gl->gen_textures_ =
      reinterpret_cast<GenTexturesProc>(::eglGetProcAddress("glGenTextures"));
  gl->delete_textures_ = reinterpret_cast<DeleteTexturesProc>(
      ::eglGetProcAddress("glDeleteTextures"));
  gl->bind_texture_ =
      reinterpret_cast<BindTextureProc>(::eglGetProcAddress("glBindTexture"));
  gl->tex_parameteri_ = reinterpret_cast<TexParameteriProc>(
      ::eglGetProcAddress("glTexParameteri"));
  gl->tex_image_2d_ =
      reinterpret_cast<TexImage2DProc>(::eglGetProcAddress("glTexImage2D"));

  if (!gl->gen_textures_ || !gl->delete_textures_ || !gl->bind_texture_ ||
      !gl->tex_parameteri_ || !gl->tex_image_2d_) {
    return nullptr;
  }

  return gl;
}

ProcTable::ProcTable() = default;

ProcTable::~ProcTable() = default;

void ProcTable::GenTextures(GLsizei n, GLuint* textures) const {
  gen_textures_(n, textures);
}

void ProcTable::DeleteTextures(GLsizei n, const GLuint* textures) const {
  delete_textures_(n, textures);
}

void ProcTable::BindTexture(GLenum target, GLuint texture) const {
  bind_texture_(target, texture);
}

void ProcTable::TexParameteri(GLenum target, GLenum pname, GLint param) const {
  tex_parameteri_(target, pname, param);
}

void ProcTable::TexImage2D(GLenum target,
                           GLint level,
                           GLint internalformat,
                           GLsizei width,
                           GLsizei height,
                           GLint border,
                           GLenum format,
                           GLenum type,
                           const void* data) const {
  tex_image_2d_(target, level, internalformat, width, height, border, format,
                type, data);
}

}  // namespace egl
}  // namespace appcode
