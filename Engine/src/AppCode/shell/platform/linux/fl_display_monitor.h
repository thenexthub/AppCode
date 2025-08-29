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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_DISPLAY_MONITOR_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_DISPLAY_MONITOR_H_

#include <gdk/gdk.h>

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlDisplayMonitor,
                     fl_display_monitor,
                     FL,
                     DISPLAY_MONITOR,
                     GObject);

/**
 * fl_display_monitor_new:
 * @engine: engine to update.
 * @display: display to monitor.
 *
 * Creates a new object to keep the engine updated with the currently used
 * displays. In GDK, a display is called a "monitor".
 *
 * Returns: a new #FlDisplayMontior.
 */
FlDisplayMonitor* fl_display_monitor_new(FlEngine* engine, GdkDisplay* display);

/**
 * fl_display_monitor_start:
 * @monitor: an #FlDisplayMonitor.
 *
 * Start monitoring for display changes.
 */
void fl_display_monitor_start(FlDisplayMonitor* monitor);

/**
 * fl_display_monitor_get_display_id:
 * @monitor: an #FlDisplayMonitor.
 * @gdk_monitor: GDK monitor to get display ID for.
 *
 * Get the ID Flutter is using for a given monitor.
 *
 * Returns: an ID or 0 if unknown.
 */
FlutterEngineDisplayId fl_display_monitor_get_display_id(
    FlDisplayMonitor* monitor,
    GdkMonitor* gdk_monitor);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_DISPLAY_MONITOR_H_
