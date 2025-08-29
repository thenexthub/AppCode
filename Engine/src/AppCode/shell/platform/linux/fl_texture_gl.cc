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

#include "appcode/shell/platform/linux/public/appcode_linux/fl_texture_gl.h"
#include "appcode/shell/platform/linux/fl_texture_gl_private.h"

#include <epoxy/gl.h>
#include <gmodule.h>
#include <cstdio>

typedef struct {
  int64_t id;
} FlTextureGLPrivate;

static void fl_texture_gl_texture_iface_init(FlTextureInterface* iface);

G_DEFINE_TYPE_WITH_CODE(FlTextureGL,
                        fl_texture_gl,
                        G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(fl_texture_get_type(),
                                              fl_texture_gl_texture_iface_init);
                        G_ADD_PRIVATE(FlTextureGL))

// Implements FlTexture::set_id
static void fl_texture_gl_set_id(FlTexture* texture, int64_t id) {
  FlTextureGL* self = FL_TEXTURE_GL(texture);
  FlTextureGLPrivate* priv = reinterpret_cast<FlTextureGLPrivate*>(
      fl_texture_gl_get_instance_private(self));
  priv->id = id;
}

// Implements FlTexture::set_id
static int64_t fl_texture_gl_get_id(FlTexture* texture) {
  FlTextureGL* self = FL_TEXTURE_GL(texture);
  FlTextureGLPrivate* priv = reinterpret_cast<FlTextureGLPrivate*>(
      fl_texture_gl_get_instance_private(self));
  return priv->id;
}

static void fl_texture_gl_texture_iface_init(FlTextureInterface* iface) {
  iface->set_id = fl_texture_gl_set_id;
  iface->get_id = fl_texture_gl_get_id;
}

static void fl_texture_gl_class_init(FlTextureGLClass* klass) {}

static void fl_texture_gl_init(FlTextureGL* self) {}

gboolean fl_texture_gl_populate(FlTextureGL* self,
                                uint32_t width,
                                uint32_t height,
                                appcodeOpenGLTexture* opengl_texture,
                                GError** error) {
  uint32_t target = 0, name = 0;
  if (!FL_TEXTURE_GL_GET_CLASS(self)->populate(self, &target, &name, &width,
                                               &height, error)) {
    return FALSE;
  }

  opengl_texture->target = target;
  opengl_texture->name = name;
  opengl_texture->format = GL_RGBA8;
  opengl_texture->destruction_callback = nullptr;
  opengl_texture->user_data = nullptr;
  opengl_texture->width = width;
  opengl_texture->height = height;

  return TRUE;
}
