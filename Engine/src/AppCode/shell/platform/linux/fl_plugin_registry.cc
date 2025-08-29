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

#include "appcode/shell/platform/linux/public/appcode_linux/fl_plugin_registry.h"

#include <gmodule.h>

G_DEFINE_INTERFACE(FlPluginRegistry, fl_plugin_registry, G_TYPE_OBJECT)

void fl_plugin_registry_default_init(FlPluginRegistryInterface* self) {}

G_MODULE_EXPORT FlPluginRegistrar* fl_plugin_registry_get_registrar_for_plugin(
    FlPluginRegistry* self,
    const gchar* name) {
  g_return_val_if_fail(FL_IS_PLUGIN_REGISTRY(self), nullptr);
  g_return_val_if_fail(name != nullptr, nullptr);

  return FL_PLUGIN_REGISTRY_GET_IFACE(self)->get_registrar_for_plugin(self,
                                                                      name);
}
