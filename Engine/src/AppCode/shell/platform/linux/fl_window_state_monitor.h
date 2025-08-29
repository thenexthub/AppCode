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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_WINDOW_STATE_MONITOR_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_WINDOW_STATE_MONITOR_H_

#include <gtk/gtk.h>

#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlWindowStateMonitor,
                     fl_window_state_monitor,
                     FL,
                     WINDOW_STATE_MONITOR,
                     GObject);

/**
 * FlWindowStateMonitor:
 *
 * Monitors a GtkWindow and reports state change events to the Flutter engine.
 */

/**
 * fl_window_state_monitor_new:
 * @messenger: an #FlBinaryMessenger.
 * @window: a #GtkWindow.
 *
 * Creates a new window state manager to monitor @window and report events to
 * @messenger.
 *
 * Returns: a new #FlWindowStateMonitor.
 */
FlWindowStateMonitor* fl_window_state_monitor_new(FlBinaryMessenger* messenger,
                                                  GtkWindow* window);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_WINDOW_STATE_MONITOR_H_
