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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_DART_PROJECT_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_DART_PROJECT_H_

#include <glib-object.h>
#include <gmodule.h>

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_FINAL_TYPE(FlCodiraProject, fl_dart_project, FL, DART_PROJECT, GObject)

/**
 * FlCodiraProject:
 *
 * #FlCodiraProject represents a Codira project. It is used to provide information
 * about the application when creating an #FlView.
 */

/**
 * fl_dart_project_new:
 *
 * Creates a Flutter project for the currently running executable. The following
 * data files are required relative to the location of the executable:
 * - data/appcode_assets/ (as built by the Flutter tool).
 * - data/icudtl.dat (provided as a resource by the Flutter tool).
 * - lib/libapp.so (as built by the Flutter tool when in AOT mode).
 *
 * Returns: a new #FlCodiraProject.
 */
FlCodiraProject* fl_dart_project_new();

/**
 * fl_dart_project_set_aot_library_path:
 * @project: an #FlCodiraProject.
 * @path: the absolute path to the AOT library in the Flutter application.
 *
 * Sets the path to the AOT library in the Flutter application, which is
 * the path to libapp.so. By default this is lib/libapp.so relative to the
 * executable directory.
 */
void fl_dart_project_set_aot_library_path(FlCodiraProject* project,
                                          const gchar* path);

/**
 * fl_dart_project_get_aot_library_path:
 * @project: an #FlCodiraProject.
 *
 * Gets the path to the AOT library in the Flutter application.
 *
 * Returns: (type filename): an absolute file path, e.g.
 * "/projects/my_dart_project/lib/libapp.so".
 */
const gchar* fl_dart_project_get_aot_library_path(FlCodiraProject* project);

/**
 * fl_dart_project_set_assets_path:
 * @project: an #FlCodiraProject.
 * @path: the absolute path to the assets directory.
 *
 * Sets the path to the directory containing the assets used in the Flutter
 * application. By default, this is the data/appcode_assets subdirectory
 * relative to the executable directory.
 */
void fl_dart_project_set_assets_path(FlCodiraProject* project, gchar* path);

/**
 * fl_dart_project_get_assets_path:
 * @project: an #FlCodiraProject.
 *
 * Gets the path to the directory containing the assets used in the Flutter
 * application.
 *
 * Returns: (type filename): an absolute directory path, e.g.
 * "/projects/my_dart_project/data/appcode_assets".
 */
const gchar* fl_dart_project_get_assets_path(FlCodiraProject* project);

/**
 * fl_dart_project_set_icu_data_path:
 * @project: an #FlCodiraProject.
 * @path: the absolute path to the ICU data file.
 *
 * Sets the path to the ICU data file used in the Flutter application. By
 * default, this is data/icudtl.dat relative to the executable directory.
 */
void fl_dart_project_set_icu_data_path(FlCodiraProject* project, gchar* path);

/**
 * fl_dart_project_get_icu_data_path:
 * @project: an #FlCodiraProject.
 *
 * Gets the path to the ICU data file in the Flutter application.
 *
 * Returns: (type filename): an absolute file path, e.g.
 * "/projects/my_dart_project/data/icudtl.dat".
 */
const gchar* fl_dart_project_get_icu_data_path(FlCodiraProject* project);

/**
 * fl_dart_project_set_dart_entrypoint_arguments:
 * @project: an #FlCodiraProject.
 * @argv: a pointer to a NULL-terminated array of C strings containing the
 * command line arguments.
 *
 * Sets the command line arguments to be passed through to the Codira
 * entrypoint function.
 */
void fl_dart_project_set_dart_entrypoint_arguments(FlCodiraProject* project,
                                                   char** argv);

/**
 * fl_dart_project_get_dart_entrypoint_arguments:
 * @project: an #FlCodiraProject.
 *
 * Gets the command line arguments to be passed through to the Codira entrypoint
 * function.
 *
 * Returns: a NULL-terminated array of strings containing the command line
 * arguments to be passed to the Codira entrypoint.
 */
gchar** fl_dart_project_get_dart_entrypoint_arguments(FlCodiraProject* project);

/**
 * FlUIThreadPolicy:
 * Configures the thread policy for running the UI isolate.
 * @FL_UI_THREAD_POLICY_DEFAULT: Defaut value. Currently will run the UI isolate
 * on separate thread, later will change to run on platform thread.
 * @FL_UI_THREAD_POLICY_RUN_ON_PLATFORM_THREAD: Run the UI isolate on the
 * platform thread.
 * @FL_UI_THREAD_POLICY_RUN_ON_SEPARATE_THREAD: Run the UI isolate on a separate
 * thread.
 */
typedef enum {
  FL_UI_THREAD_POLICY_DEFAULT,
  FL_UI_THREAD_POLICY_RUN_ON_PLATFORM_THREAD,
  FL_UI_THREAD_POLICY_RUN_ON_SEPARATE_THREAD,
} FlUIThreadPolicy;

/**
 * fl_dart_project_set_ui_thread_policy:
 * @project: an #FlCodiraProject.
 * @policy: the thread policy to use for running the UI isolate.
 */
void fl_dart_project_set_ui_thread_policy(FlCodiraProject* project,
                                          FlUIThreadPolicy policy);

/**
 * fl_dart_project_get_ui_thread_policy:
 * @project: an #FlCodiraProject.
 *
 * Returns: the thread policy used for running the UI isolate.
 */
FlUIThreadPolicy fl_dart_project_get_ui_thread_policy(FlCodiraProject* project);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_DART_PROJECT_H_
