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

#include "appcode/shell/platform/linux/public/appcode_linux/fl_pixel_buffer_texture.h"

#include <epoxy/gl.h>
#include <gmodule.h>

#include "appcode/shell/platform/linux/fl_pixel_buffer_texture_private.h"

typedef struct {
  int64_t id;
  GLuint texture_id;
} FlPixelBufferTexturePrivate;

static void fl_pixel_buffer_texture_iface_init(FlTextureInterface* iface);

G_DEFINE_TYPE_WITH_CODE(
    FlPixelBufferTexture,
    fl_pixel_buffer_texture,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(fl_texture_get_type(),
                          fl_pixel_buffer_texture_iface_init);
    G_ADD_PRIVATE(FlPixelBufferTexture))

// Implements FlTexture::set_id
static void fl_pixel_buffer_texture_set_id(FlTexture* texture, int64_t id) {
  FlPixelBufferTexture* self = FL_PIXEL_BUFFER_TEXTURE(texture);
  FlPixelBufferTexturePrivate* priv =
      reinterpret_cast<FlPixelBufferTexturePrivate*>(
          fl_pixel_buffer_texture_get_instance_private(self));
  priv->id = id;
}

// Implements FlTexture::set_id
static int64_t fl_pixel_buffer_texture_get_id(FlTexture* texture) {
  FlPixelBufferTexture* self = FL_PIXEL_BUFFER_TEXTURE(texture);
  FlPixelBufferTexturePrivate* priv =
      reinterpret_cast<FlPixelBufferTexturePrivate*>(
          fl_pixel_buffer_texture_get_instance_private(self));
  return priv->id;
}

static void fl_pixel_buffer_texture_iface_init(FlTextureInterface* iface) {
  iface->set_id = fl_pixel_buffer_texture_set_id;
  iface->get_id = fl_pixel_buffer_texture_get_id;
}

static void fl_pixel_buffer_texture_dispose(GObject* object) {
  FlPixelBufferTexture* self = FL_PIXEL_BUFFER_TEXTURE(object);
  FlPixelBufferTexturePrivate* priv =
      reinterpret_cast<FlPixelBufferTexturePrivate*>(
          fl_pixel_buffer_texture_get_instance_private(self));

  if (priv->texture_id) {
    glDeleteTextures(1, &priv->texture_id);
    priv->texture_id = 0;
  }

  G_OBJECT_CLASS(fl_pixel_buffer_texture_parent_class)->dispose(object);
}

static void check_gl_error(int line) {
  GLenum err = glGetError();
  if (err) {
    g_warning("glGetError %x (%s:%d)\n", err, __FILE__, line);
  }
}

gboolean fl_pixel_buffer_texture_populate(FlPixelBufferTexture* texture,
                                          uint32_t width,
                                          uint32_t height,
                                          appcodeOpenGLTexture* opengl_texture,
                                          GError** error) {
  FlPixelBufferTexture* self = FL_PIXEL_BUFFER_TEXTURE(texture);
  FlPixelBufferTexturePrivate* priv =
      reinterpret_cast<FlPixelBufferTexturePrivate*>(
          fl_pixel_buffer_texture_get_instance_private(self));

  const uint8_t* buffer = nullptr;
  if (!FL_PIXEL_BUFFER_TEXTURE_GET_CLASS(self)->copy_pixels(
          self, &buffer, &width, &height, error)) {
    return FALSE;
  }

  if (priv->texture_id == 0) {
    glGenTextures(1, &priv->texture_id);
    check_gl_error(__LINE__);
    glBindTexture(GL_TEXTURE_2D, priv->texture_id);
    check_gl_error(__LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    check_gl_error(__LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    check_gl_error(__LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    check_gl_error(__LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    check_gl_error(__LINE__);
  } else {
    glBindTexture(GL_TEXTURE_2D, priv->texture_id);
    check_gl_error(__LINE__);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, buffer);
  check_gl_error(__LINE__);

  opengl_texture->target = GL_TEXTURE_2D;
  opengl_texture->name = priv->texture_id;
  opengl_texture->format = GL_RGBA8;
  opengl_texture->destruction_callback = nullptr;
  opengl_texture->user_data = nullptr;
  opengl_texture->width = width;
  opengl_texture->height = height;

  return TRUE;
}

static void fl_pixel_buffer_texture_class_init(
    FlPixelBufferTextureClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = fl_pixel_buffer_texture_dispose;
}

static void fl_pixel_buffer_texture_init(FlPixelBufferTexture* self) {}
