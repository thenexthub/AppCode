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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_SETTINGS_PORTAL_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_SETTINGS_PORTAL_H_

#include "appcode/shell/platform/linux/fl_settings.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlSettingsPortal,
                     fl_settings_portal,
                     FL,
                     SETTINGS_PORTAL,
                     GObject);

/**
 * FlSettingsPortal:
 * #FlSettingsPortal reads settings from the XDG desktop portal.
 */

/**
 * fl_settings_portal_new:
 *
 * Creates a new settings portal instance.
 *
 * Returns: a new #FlSettingsPortal.
 */
FlSettingsPortal* fl_settings_portal_new();

/**
 * fl_settings_portal_new_with_values:
 * @values: (nullable): a #GVariantDict.
 *
 * Creates a new settings portal instance with initial values for testing.
 *
 * Returns: a new #FlSettingsPortal.
 */
FlSettingsPortal* fl_settings_portal_new_with_values(GVariantDict* values);

/**
 * fl_settings_portal_start:
 * @portal: an #FlSettingsPortal.
 * @error: (allow-none): #GError location to store the error occurring, or %NULL
 *
 * Reads the current settings and starts monitoring for changes in the desktop
 * portal settings.
 *
 * Returns: %TRUE on success, or %FALSE if the portal is not available.
 */
gboolean fl_settings_portal_start(FlSettingsPortal* portal, GError** error);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_SETTINGS_PORTAL_H_
