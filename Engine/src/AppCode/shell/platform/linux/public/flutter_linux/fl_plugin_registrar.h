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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_PLUGIN_REGISTRAR_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_PLUGIN_REGISTRAR_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <glib-object.h>
#include <gmodule.h>

#include "fl_binary_messenger.h"
#include "fl_texture_registrar.h"
#include "fl_view.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_INTERFACE(FlPluginRegistrar,
                    fl_plugin_registrar,
                    FL,
                    PLUGIN_REGISTRAR,
                    GObject)

struct _FlPluginRegistrarInterface {
  GTypeInterface parent_iface;

  FlBinaryMessenger* (*get_messenger)(FlPluginRegistrar* registrar);

  FlTextureRegistrar* (*get_texture_registrar)(FlPluginRegistrar* registrar);

  FlView* (*get_view)(FlPluginRegistrar* registrar);
};

/**
 * FlPluginRegistrar:
 *
 * #FlPluginRegistrar is used when registering new plugins.
 */

/**
 * fl_plugin_registrar_get_messenger:
 * @registrar: an #FlPluginRegistrar.
 *
 * Gets the messenger this plugin can communicate with.
 *
 * Returns: an #FlBinaryMessenger.
 */
FlBinaryMessenger* fl_plugin_registrar_get_messenger(
    FlPluginRegistrar* registrar);

/**
 * fl_plugin_registrar_get_texture_registrar:
 * @registrar: an #FlPluginRegistrar.
 *
 * Gets the texture registrar this plugin can communicate with.
 *
 * Returns: an #FlTextureRegistrar.
 */
FlTextureRegistrar* fl_plugin_registrar_get_texture_registrar(
    FlPluginRegistrar* registrar);

/**
 * fl_plugin_registrar_get_view:
 * @registrar: an #FlPluginRegistrar.
 *
 * Get the view that Flutter is rendering with.
 *
 * Returns: (allow-none): an #FlView or %NULL if running in headless mode.
 */
FlView* fl_plugin_registrar_get_view(FlPluginRegistrar* registrar);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_PLUGIN_REGISTRAR_H_
