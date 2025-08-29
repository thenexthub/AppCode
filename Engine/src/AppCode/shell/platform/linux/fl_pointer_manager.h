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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_POINTER_MANAGER_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_POINTER_MANAGER_H_

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlPointerManager,
                     fl_pointer_manager,
                     FL,
                     POINTER_MANAGER,
                     GObject);

/**
 * fl_pointer_manager_new:
 * @view_id: view ID to report events for.
 * @engine: an #FlEngine.
 *
 * Create a new #FlPointerManager.
 *
 * Returns: a new #FlPointerManager.
 */
FlPointerManager* fl_pointer_manager_new(FlutterViewId view_id,
                                         FlEngine* engine);

/**
 * fl_pointer_manager_handle_button_press:
 * @manager: an #FlPointerManager.
 * @event_time: event time in milliseconds.
 * @device_kind: kind of device generating the event.
 * @x: x co-ordinate of event.
 * @y: y co-ordinate of event.
 * @gdk_button: button being pressed.
 *
 * Returns %TRUE if this event was handled.
 */
gboolean fl_pointer_manager_handle_button_press(
    FlPointerManager* manager,
    guint event_time,
    FlutterPointerDeviceKind device_kind,
    gdouble x,
    gdouble y,
    guint gdk_button);

/**
 * fl_pointer_manager_handle_button_release:
 * @manager: an #FlPointerManager.
 * @event_time: event time in milliseconds.
 * @device_kind: kind of device generating the event.
 * @x: x co-ordinate of event.
 * @y: y co-ordinate of event.
 * @gdk_button: button being released.
 *
 * Returns %TRUE if this event was handled.
 */
gboolean fl_pointer_manager_handle_button_release(
    FlPointerManager* manager,
    guint event_time,
    FlutterPointerDeviceKind device_kind,
    gdouble x,
    gdouble y,
    guint gdk_button);

/**
 * fl_pointer_manager_handle_motion:
 * @manager: an #FlPointerManager.
 * @event_time: event time in milliseconds.
 * @device_kind: kind of device generating the event.
 * @x: x co-ordinate of event.
 * @y: y co-ordinate of event.
 *
 * Returns %TRUE if this event was handled.
 */
gboolean fl_pointer_manager_handle_motion(FlPointerManager* manager,
                                          guint event_time,
                                          FlutterPointerDeviceKind device_kind,
                                          gdouble x,
                                          gdouble y);

/**
 * fl_pointer_manager_handle_enter:
 * @manager: an #FlPointerManager.
 * @event_time: event time in milliseconds.
 * @device_kind: kind of device generating the event.
 * @x: x co-ordinate of event.
 * @y: y co-ordinate of event.
 *
 * Returns %TRUE if this event was handled.
 */
gboolean fl_pointer_manager_handle_enter(FlPointerManager* manager,
                                         guint event_time,
                                         FlutterPointerDeviceKind device_kind,
                                         gdouble x,
                                         gdouble y);

/**
 * fl_pointer_manager_handle_leave:
 * @manager: an #FlPointerManager.
 * @event_time: event time in milliseconds.
 * @device_kind: kind of device generating the event.
 * @x: x co-ordinate of event.
 * @y: y co-ordinate of event.
 *
 * Returns %TRUE if this event was handled.
 */
gboolean fl_pointer_manager_handle_leave(FlPointerManager* manager,
                                         guint event_time,
                                         FlutterPointerDeviceKind device_kind,
                                         gdouble x,
                                         gdouble y);
G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_POINTER_MANAGER_H_
