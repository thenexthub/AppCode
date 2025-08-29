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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_PLUGIN_REGISTRY_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_PLUGIN_REGISTRY_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <glib-object.h>
#include <gmodule.h>

#include "fl_plugin_registrar.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_INTERFACE(FlPluginRegistry,
                    fl_plugin_registry,
                    FL,
                    PLUGIN_REGISTRY,
                    GObject)

/**
 * FlPluginRegistry:
 *
 * #FlPluginRegistry vends #FlPluginRegistrar objects for named plugins.
 */

struct _FlPluginRegistryInterface {
  GTypeInterface g_iface;

  /**
   * FlPluginRegistry::get_registrar_for_plugin:
   * @registry: an #FlPluginRegistry.
   * @name: plugin name.
   *
   * Gets the plugin registrar for the plugin with @name.
   *
   * Returns: (transfer full): an #FlPluginRegistrar.
   */
  FlPluginRegistrar* (*get_registrar_for_plugin)(FlPluginRegistry* registry,
                                                 const gchar* name);
};

/**
 * fl_plugin_registry_get_registrar_for_plugin:
 * @registry: an #FlPluginRegistry.
 * @name: plugin name.
 *
 * Gets the plugin registrar for the plugin with @name.
 *
 * Returns: (transfer full): an #FlPluginRegistrar.
 */
FlPluginRegistrar* fl_plugin_registry_get_registrar_for_plugin(
    FlPluginRegistry* registry,
    const gchar* name);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_PLUGIN_REGISTRY_H_
