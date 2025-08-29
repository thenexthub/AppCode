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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_KEY_EVENT_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_KEY_EVENT_H_

#include <stdint.h>

#include <gdk/gdk.h>

G_DECLARE_FINAL_TYPE(FlKeyEvent, fl_key_event, FL, KEY_EVENT, GObject);

/**
 * FlKeyEvent:
 * A struct that stores information from GdkEvent.
 *
 * This is a class only used within the GTK embedding, created by
 * FlView and consumed by FlKeyboardHandler. It is not sent to
 * the embedder.
 *
 * This object contains information from GdkEvent as well as an origin event
 * object, so that Flutter can create an event object in unit tests even after
 * migrating to GDK 4.0 which stops supporting creating GdkEvent.
 */

FlKeyEvent* fl_key_event_new(guint32 time,
                             gboolean is_press,
                             guint16 keycode,
                             guint keyval,
                             GdkModifierType state,
                             guint8 group);

/**
 * fl_key_event_new_from_gdk_event:
 * @event: the #GdkEvent this #FlKeyEvent is based on.
 *
 * Create a new #FlKeyEvent based on a #GdkEvent.
 *
 * Returns: a new #FlKeyEvent.
 */
FlKeyEvent* fl_key_event_new_from_gdk_event(GdkEvent* event);

guint32 fl_key_event_get_time(FlKeyEvent* event);

gboolean fl_key_event_get_is_press(FlKeyEvent* event);

guint16 fl_key_event_get_keycode(FlKeyEvent* event);

guint fl_key_event_get_keyval(FlKeyEvent* event);

GdkModifierType fl_key_event_get_state(FlKeyEvent* event);

guint8 fl_key_event_get_group(FlKeyEvent* event);

GdkEvent* fl_key_event_get_origin(FlKeyEvent* event);

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_KEY_EVENT_H_
