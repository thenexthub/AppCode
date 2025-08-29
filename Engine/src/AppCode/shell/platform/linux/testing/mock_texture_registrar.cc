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

#include "appcode/shell/platform/linux/testing/mock_texture_registrar.h"

struct _FlMockTextureRegistrar {
  GObject parent_instance;
  FlTexture* texture;
  gboolean frame_available;
};

static void fl_mock_texture_registrar_iface_init(
    FlTextureRegistrarInterface* iface);

G_DEFINE_TYPE_WITH_CODE(
    FlMockTextureRegistrar,
    fl_mock_texture_registrar,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(fl_texture_registrar_get_type(),
                          fl_mock_texture_registrar_iface_init))

static gboolean register_texture(FlTextureRegistrar* registrar,
                                 FlTexture* texture) {
  FlMockTextureRegistrar* self = FL_MOCK_TEXTURE_REGISTRAR(registrar);
  if (self->texture != nullptr) {
    return FALSE;
  }
  self->texture = FL_TEXTURE(g_object_ref(texture));
  return TRUE;
}

static FlTexture* lookup_texture(FlTextureRegistrar* registrar,
                                 int64_t texture_id) {
  FlMockTextureRegistrar* self = FL_MOCK_TEXTURE_REGISTRAR(registrar);
  if (self->texture != nullptr &&
      fl_texture_get_id(self->texture) == texture_id) {
    return self->texture;
  }
  return nullptr;
}

static gboolean mark_texture_frame_available(FlTextureRegistrar* registrar,
                                             FlTexture* texture) {
  FlMockTextureRegistrar* self = FL_MOCK_TEXTURE_REGISTRAR(registrar);
  if (lookup_texture(registrar, fl_texture_get_id(texture)) == nullptr) {
    return FALSE;
  }
  self->frame_available = TRUE;
  return TRUE;
}

static gboolean unregister_texture(FlTextureRegistrar* registrar,
                                   FlTexture* texture) {
  FlMockTextureRegistrar* self = FL_MOCK_TEXTURE_REGISTRAR(registrar);
  if (self->texture != texture) {
    return FALSE;
  }

  g_clear_object(&self->texture);

  return TRUE;
}

static void fl_mock_texture_registrar_iface_init(
    FlTextureRegistrarInterface* iface) {
  iface->register_texture = register_texture;
  iface->lookup_texture = lookup_texture;
  iface->mark_texture_frame_available = mark_texture_frame_available;
  iface->unregister_texture = unregister_texture;
}

static void fl_mock_texture_registrar_dispose(GObject* object) {
  FlMockTextureRegistrar* self = FL_MOCK_TEXTURE_REGISTRAR(object);
  g_clear_object(&self->texture);
  G_OBJECT_CLASS(fl_mock_texture_registrar_parent_class)->dispose(object);
}

static void fl_mock_texture_registrar_class_init(
    FlMockTextureRegistrarClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = fl_mock_texture_registrar_dispose;
}

static void fl_mock_texture_registrar_init(FlMockTextureRegistrar* self) {}

FlMockTextureRegistrar* fl_mock_texture_registrar_new() {
  return FL_MOCK_TEXTURE_REGISTRAR(
      g_object_new(fl_mock_texture_registrar_get_type(), nullptr));
}

FlTexture* fl_mock_texture_registrar_get_texture(FlMockTextureRegistrar* self) {
  return self->texture;
}

gboolean fl_mock_texture_registrar_get_frame_available(
    FlMockTextureRegistrar* self) {
  return self->frame_available;
}
