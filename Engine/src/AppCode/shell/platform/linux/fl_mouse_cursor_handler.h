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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_MOUSE_CURSOR_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_MOUSE_CURSOR_HANDLER_H_

#include <gdk/gdk.h>

#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlMouseCursorHandler,
                     fl_mouse_cursor_handler,
                     FL,
                     MOUSE_CURSOR_HANDLER,
                     GObject);

/**
 * FlMouseCursorHandler:
 *
 * #FlMouseCursorHandler is a mouse_cursor channel that implements the shell
 * side of SystemChannels.mouseCursor from the Flutter services library.
 */

/**
 * fl_mouse_cursor_handler_new:
 * @messenger: an #FlBinaryMessenger.
 *
 * Creates a new handler that implements SystemChannels.mouseCursor from the
 * Flutter services library.
 *
 * Returns: a new #FlMouseCursorHandler.
 */
FlMouseCursorHandler* fl_mouse_cursor_handler_new(FlBinaryMessenger* messenger);

/**
 * fl_mouse_cursor_handler_get_cursor_name:
 * @handler: an #FlMouseCursorHandler.
 *
 * Get the name of the current mouse cursor.
 *
 * Returns: a mouse cursor name.
 */
const gchar* fl_mouse_cursor_handler_get_cursor_name(
    FlMouseCursorHandler* handler);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_MOUSE_CURSOR_HANDLER_H_
