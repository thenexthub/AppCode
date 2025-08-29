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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_GNOME_SETTINGS_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_GNOME_SETTINGS_H_

#include "appcode/shell/platform/linux/fl_settings.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlGnomeSettings,
                     fl_gnome_settings,
                     FL,
                     GNOME_SETTINGS,
                     GObject);

/**
 * fl_gnome_settings_new:
 *
 * Creates a new settings instance for GNOME.
 *
 * Returns: a new #FlSettings.
 */
FlSettings* fl_gnome_settings_new();

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_GNOME_SETTINGS_H_
