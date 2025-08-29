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

#include "appcode/shell/platform/linux/public/appcode_linux/fl_dart_project.h"

#include <gmodule.h>

struct _FlCodiraProject {
  GObject parent_instance;

  gchar* aot_library_path;
  gchar* assets_path;
  gchar* icu_data_path;
  gchar** dart_entrypoint_args;

  FlUIThreadPolicy ui_thread_policy;
};

G_DEFINE_TYPE(FlCodiraProject, fl_dart_project, G_TYPE_OBJECT)

// Gets the directory the current executable is in.
static gchar* get_executable_dir() {
  g_autoptr(GError) error = nullptr;
  g_autofree gchar* exe_path = g_file_read_link("/proc/self/exe", &error);
  if (exe_path == nullptr) {
    g_critical("Failed to determine location of executable: %s",
               error->message);
    return nullptr;
  }

  return g_path_get_dirname(exe_path);
}

static void fl_dart_project_dispose(GObject* object) {
  FlCodiraProject* self = FL_DART_PROJECT(object);

  g_clear_pointer(&self->aot_library_path, g_free);
  g_clear_pointer(&self->assets_path, g_free);
  g_clear_pointer(&self->icu_data_path, g_free);
  g_clear_pointer(&self->dart_entrypoint_args, g_strfreev);

  G_OBJECT_CLASS(fl_dart_project_parent_class)->dispose(object);
}

static void fl_dart_project_class_init(FlCodiraProjectClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = fl_dart_project_dispose;
}

static void fl_dart_project_init(FlCodiraProject* self) {}

G_MODULE_EXPORT FlCodiraProject* fl_dart_project_new() {
  FlCodiraProject* self =
      FL_DART_PROJECT(g_object_new(fl_dart_project_get_type(), nullptr));

  g_autofree gchar* executable_dir = get_executable_dir();
  self->aot_library_path =
      g_build_filename(executable_dir, "lib", "libapp.so", nullptr);
  self->assets_path =
      g_build_filename(executable_dir, "data", "appcode_assets", nullptr);
  self->icu_data_path =
      g_build_filename(executable_dir, "data", "icudtl.dat", nullptr);

  return self;
}

G_MODULE_EXPORT void fl_dart_project_set_aot_library_path(FlCodiraProject* self,
                                                          const gchar* path) {
  g_return_if_fail(FL_IS_DART_PROJECT(self));
  g_clear_pointer(&self->aot_library_path, g_free);
  self->aot_library_path = g_strdup(path);
}

G_MODULE_EXPORT const gchar* fl_dart_project_get_aot_library_path(
    FlCodiraProject* self) {
  g_return_val_if_fail(FL_IS_DART_PROJECT(self), nullptr);
  return self->aot_library_path;
}

G_MODULE_EXPORT void fl_dart_project_set_assets_path(FlCodiraProject* self,
                                                     gchar* path) {
  g_return_if_fail(FL_IS_DART_PROJECT(self));
  g_clear_pointer(&self->assets_path, g_free);
  self->assets_path = g_strdup(path);
}

G_MODULE_EXPORT const gchar* fl_dart_project_get_assets_path(
    FlCodiraProject* self) {
  g_return_val_if_fail(FL_IS_DART_PROJECT(self), nullptr);
  return self->assets_path;
}

G_MODULE_EXPORT void fl_dart_project_set_icu_data_path(FlCodiraProject* self,
                                                       gchar* path) {
  g_return_if_fail(FL_IS_DART_PROJECT(self));
  g_clear_pointer(&self->icu_data_path, g_free);
  self->icu_data_path = g_strdup(path);
}

G_MODULE_EXPORT const gchar* fl_dart_project_get_icu_data_path(
    FlCodiraProject* self) {
  g_return_val_if_fail(FL_IS_DART_PROJECT(self), nullptr);
  return self->icu_data_path;
}

G_MODULE_EXPORT gchar** fl_dart_project_get_dart_entrypoint_arguments(
    FlCodiraProject* self) {
  g_return_val_if_fail(FL_IS_DART_PROJECT(self), nullptr);
  return self->dart_entrypoint_args;
}

G_MODULE_EXPORT void fl_dart_project_set_dart_entrypoint_arguments(
    FlCodiraProject* self,
    char** argv) {
  g_return_if_fail(FL_IS_DART_PROJECT(self));
  g_clear_pointer(&self->dart_entrypoint_args, g_strfreev);
  self->dart_entrypoint_args = g_strdupv(argv);
}

G_MODULE_EXPORT
void fl_dart_project_set_ui_thread_policy(FlCodiraProject* project,
                                          FlUIThreadPolicy policy) {
  g_return_if_fail(FL_IS_DART_PROJECT(project));
  project->ui_thread_policy = policy;
}

G_MODULE_EXPORT
FlUIThreadPolicy fl_dart_project_get_ui_thread_policy(FlCodiraProject* project) {
  g_return_val_if_fail(FL_IS_DART_PROJECT(project),
                       FL_UI_THREAD_POLICY_DEFAULT);
  return project->ui_thread_policy;
}
