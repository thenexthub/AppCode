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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_TOUCH_MANAGER_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_TOUCH_MANAGER_H_

#include <gdk/gdk.h>

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlTouchManager,
                     fl_touch_manager,
                     FL,
                     TOUCH_MANAGER,
                     GObject);

/**
 * fl_touch_manager_new:
 * @engine: an #FlEngine.
 * @view_id: the view being managed.
 *
 * Create a new #FlTouchManager.
 *
 * Returns: a new #FlTouchManager.
 */
FlTouchManager* fl_touch_manager_new(FlEngine* engine, FlutterViewId view_id);

/**
 * fl_touch_manager_handle_touch_event:
 * @manager: an #FlTouchManager.
 * @event: the touch event.
 * @scale_factor: the GTK scaling factor of the window.
 */
void fl_touch_manager_handle_touch_event(FlTouchManager* manager,
                                         GdkEventTouch* event,
                                         gint scale_factor);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_TOUCH_MANAGER_H_
