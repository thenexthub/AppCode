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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_MOUSE_CURSOR_CHANNEL_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_MOUSE_CURSOR_CHANNEL_H_

#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlMouseCursorChannel,
                     fl_mouse_cursor_channel,
                     FL,
                     MOUSE_CURSOR_CHANNEL,
                     GObject);

/**
 * FlMouseCursorChannel:
 *
 * #FlMouseCursorChannel is a cursor channel that implements the shell
 * side of SystemChannels.mouseCursor from the Flutter services library.
 */

typedef struct {
  void (*activate_system_cursor)(const gchar* kind, gpointer user_data);
} FlMouseCursorChannelVTable;

/**
 * fl_mouse_cursor_channel_new:
 * @messenger: an #FlBinaryMessenger.
 * @vtable: callbacks for incoming method calls.
 * @user_data: data to pass in callbacks.
 *
 * Creates a new channel that implements SystemChannels.mouseCursor from the
 * Flutter services library.
 *
 * Returns: a new #FlMouseCursorChannel.
 */
FlMouseCursorChannel* fl_mouse_cursor_channel_new(
    FlBinaryMessenger* messenger,
    FlMouseCursorChannelVTable* vtable,
    gpointer user_data);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_MOUSE_CURSOR_CHANNEL_H_
