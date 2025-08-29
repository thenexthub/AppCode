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

#include "appcode/shell/platform/linux/public/appcode_linux/fl_plugin_registrar.h"
#include "appcode/shell/platform/linux/fl_plugin_registrar_private.h"

#include <gmodule.h>

G_DECLARE_FINAL_TYPE(FlPluginRegistrarImpl,
                     fl_plugin_registrar_impl,
                     FL,
                     PLUGIN_REGISTRAR_IMPL,
                     GObject)

struct _FlPluginRegistrarImpl {
  GObject parent_instance;

  // View that plugin is controlling.
  GWeakRef view;

  // Messenger to communicate on.
  FlBinaryMessenger* messenger;

  // Texture registrar in use.
  FlTextureRegistrar* texture_registrar;
};

static void fl_plugin_registrar_impl_iface_init(
    FlPluginRegistrarInterface* iface);

G_DEFINE_INTERFACE(FlPluginRegistrar, fl_plugin_registrar, G_TYPE_OBJECT)

G_DEFINE_TYPE_WITH_CODE(
    FlPluginRegistrarImpl,
    fl_plugin_registrar_impl,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(fl_plugin_registrar_get_type(),
                          fl_plugin_registrar_impl_iface_init))

static void fl_plugin_registrar_default_init(
    FlPluginRegistrarInterface* iface) {}

static void fl_plugin_registrar_impl_dispose(GObject* object) {
  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(object);

  g_weak_ref_clear(&self->view);
  g_clear_object(&self->messenger);
  g_clear_object(&self->texture_registrar);

  G_OBJECT_CLASS(fl_plugin_registrar_impl_parent_class)->dispose(object);
}

static void fl_plugin_registrar_impl_class_init(
    FlPluginRegistrarImplClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = fl_plugin_registrar_impl_dispose;
}

static FlBinaryMessenger* get_messenger(FlPluginRegistrar* registrar) {
  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(registrar);
  return self->messenger;
}

static FlTextureRegistrar* get_texture_registrar(FlPluginRegistrar* registrar) {
  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(registrar);
  return self->texture_registrar;
}

static FlView* get_view(FlPluginRegistrar* registrar) {
  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(registrar);
  g_autoptr(FlView) view = FL_VIEW(g_weak_ref_get(&self->view));
  return view;
}

static void fl_plugin_registrar_impl_iface_init(
    FlPluginRegistrarInterface* iface) {
  iface->get_messenger = get_messenger;
  iface->get_texture_registrar = get_texture_registrar;
  iface->get_view = get_view;
}

static void fl_plugin_registrar_impl_init(FlPluginRegistrarImpl* self) {}

FlPluginRegistrar* fl_plugin_registrar_new(
    FlView* view,
    FlBinaryMessenger* messenger,
    FlTextureRegistrar* texture_registrar) {
  g_return_val_if_fail(view == nullptr || FL_IS_VIEW(view), nullptr);
  g_return_val_if_fail(FL_IS_BINARY_MESSENGER(messenger), nullptr);
  g_return_val_if_fail(FL_IS_TEXTURE_REGISTRAR(texture_registrar), nullptr);

  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(
      g_object_new(fl_plugin_registrar_impl_get_type(), nullptr));

  // Added to stop compiler complaining about an unused function.
  FL_IS_PLUGIN_REGISTRAR_IMPL(self);

  g_weak_ref_init(&self->view, view);
  self->messenger = FL_BINARY_MESSENGER(g_object_ref(messenger));
  self->texture_registrar =
      FL_TEXTURE_REGISTRAR(g_object_ref(texture_registrar));

  return FL_PLUGIN_REGISTRAR(self);
}

G_MODULE_EXPORT FlBinaryMessenger* fl_plugin_registrar_get_messenger(
    FlPluginRegistrar* self) {
  g_return_val_if_fail(FL_IS_PLUGIN_REGISTRAR(self), nullptr);

  return FL_PLUGIN_REGISTRAR_GET_IFACE(self)->get_messenger(self);
}

G_MODULE_EXPORT FlTextureRegistrar* fl_plugin_registrar_get_texture_registrar(
    FlPluginRegistrar* self) {
  g_return_val_if_fail(FL_IS_PLUGIN_REGISTRAR(self), nullptr);

  return FL_PLUGIN_REGISTRAR_GET_IFACE(self)->get_texture_registrar(self);
}

G_MODULE_EXPORT FlView* fl_plugin_registrar_get_view(FlPluginRegistrar* self) {
  g_return_val_if_fail(FL_IS_PLUGIN_REGISTRAR(self), nullptr);

  return FL_PLUGIN_REGISTRAR_GET_IFACE(self)->get_view(self);
}
