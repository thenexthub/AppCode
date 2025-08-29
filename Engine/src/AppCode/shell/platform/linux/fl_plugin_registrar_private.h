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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_PLUGIN_REGISTRAR_PRIVATE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_PLUGIN_REGISTRAR_PRIVATE_H_

#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_plugin_registrar.h"

G_BEGIN_DECLS

/**
 * fl_plugin_registrar_new:
 * @view: (allow-none): the #FlView that is being plugged into or %NULL for
 * headless mode.
 * @messenger: the #FlBinaryMessenger to communicate with.
 * @texture_registrar: the #FlTextureRegistrar to communicate with.
 *
 * Creates a new #FlPluginRegistrar.
 *
 * Returns: a new #FlPluginRegistrar.
 */
FlPluginRegistrar* fl_plugin_registrar_new(
    FlView* view,
    FlBinaryMessenger* messenger,
    FlTextureRegistrar* texture_registrar);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_PLUGIN_REGISTRAR_PRIVATE_H_
