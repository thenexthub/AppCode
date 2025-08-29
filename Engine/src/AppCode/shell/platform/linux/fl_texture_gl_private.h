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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_TEXTURE_GL_PRIVATE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_TEXTURE_GL_PRIVATE_H_

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_texture_gl.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_texture_registrar.h"

G_BEGIN_DECLS

/**
 * fl_texture_gl_populate:
 * @texture: an #FlTextureGL.
 * @width: width of the texture.
 * @height: height of the texture.
 * @opengl_texture: (out): return an #FlutterOpenGLTexture.
 * @error: (allow-none): #GError location to store the error occurring, or
 * %NULL to ignore.
 *
 * Attempts to populate the specified @opengl_texture with texture details
 * such as the name, width, height and the pixel format.
 *
 * Returns: %TRUE on success.
 */
gboolean fl_texture_gl_populate(FlTextureGL* texture,
                                uint32_t width,
                                uint32_t height,
                                FlutterOpenGLTexture* opengl_texture,
                                GError** error);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_TEXTURE_GL_PRIVATE_H_
