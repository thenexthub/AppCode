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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_SETTINGS_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_SETTINGS_HANDLER_H_

#include "appcode/shell/platform/linux/fl_settings.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlSettingsHandler,
                     fl_settings_handler,
                     FL,
                     SETTINGS_HANDLER,
                     GObject);

/**
 * FlSettingsHandler:
 *
 * #FlSettingsHandler is a handler that implements the Flutter user settings
 * channel.
 */

/**
 * fl_settings_handler_new:
 * @messenger: an #FlEngine
 *
 * Creates a new handler that sends user settings to the Flutter engine.
 *
 * Returns: a new #FlSettingsHandler
 */
FlSettingsHandler* fl_settings_handler_new(FlEngine* engine);

/**
 * fl_settings_handler_start:
 * @handler: an #FlSettingsHandler.
 *
 * Sends the current settings to the engine and updates when they change.
 */
void fl_settings_handler_start(FlSettingsHandler* handler,
                               FlSettings* settings);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_SETTINGS_HANDLER_H_
