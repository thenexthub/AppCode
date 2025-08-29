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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_FRAMEBUFFER_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_FRAMEBUFFER_H_

#include <epoxy/gl.h>
#include <glib-object.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlFramebuffer, fl_framebuffer, FL, FRAMEBUFFER, GObject)

/**
 * FlFramebuffer:
 *
 * #FlFramebuffer creates framebuffers and their backing textures
 * for use by the Flutter compositor.
 */

/**
 * fl_framebuffer_new:
 * @format: format, e.g. GL_RGB, GL_BGR
 * @width: width of texture.
 * @height: height of texture.
 *
 * Creates a new frame buffer. Requires a valid OpenGL context to create.
 *
 * Returns: a new #FlFramebuffer.
 */
FlFramebuffer* fl_framebuffer_new(GLint format, size_t width, size_t height);

/**
 * fl_framebuffer_get_id:
 * @framebuffer: an #FlFramebuffer.
 *
 * Gets the ID for this framebuffer.
 *
 * Returns: OpenGL framebuffer id or 0 if creation failed.
 */
GLuint fl_framebuffer_get_id(FlFramebuffer* framebuffer);

/**
 * fl_framebuffer_get_texture_id:
 * @framebuffer: an #FlFramebuffer.
 *
 * Gets the ID of the texture associated with this framebuffer.
 *
 * Returns: OpenGL texture id or 0 if creation failed.
 */
GLuint fl_framebuffer_get_texture_id(FlFramebuffer* framebuffer);

/**
 * fl_framebuffer_get_target:
 * @framebuffer: an #FlFramebuffer.
 *
 * Gets target texture (example GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE).
 *
 * Returns: target texture.
 */
GLenum fl_framebuffer_get_target(FlFramebuffer* framebuffer);

/**
 * fl_framebuffer_get_width:
 * @framebuffer: an #FlFramebuffer.
 *
 * Gets the width of the framebuffer in pixels.
 *
 * Returns: width in pixels.
 */
size_t fl_framebuffer_get_width(FlFramebuffer* framebuffer);

/**
 * fl_framebuffer_get_height:
 * @framebuffer: an #FlFramebuffer.
 *
 * Gets the height of the framebuffer in pixels.
 *
 * Returns: height in pixels.
 */
size_t fl_framebuffer_get_height(FlFramebuffer* framebuffer);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_FRAMEBUFFER_H_
