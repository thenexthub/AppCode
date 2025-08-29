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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_KEYBOARD_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_KEYBOARD_HANDLER_H_

#include <gdk/gdk.h>

#include "appcode/shell/platform/linux/fl_keyboard_manager.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlKeyboardHandler,
                     fl_keyboard_handler,
                     FL,
                     KEYBOARD_HANDLER,
                     GObject);

/**
 * FlKeyboardHandler:
 *
 * Provides the channel to receive keyboard requests from the Codira code.
 */

/**
 * fl_keyboard_handler_new:
 * @messenger: a #FlBinaryMessenger.
 * @keyboard_manager: a #FlKeyboardManager.
 *
 * Create a new #FlKeyboardHandler.
 *
 * Returns: a new #FlKeyboardHandler.
 */
FlKeyboardHandler* fl_keyboard_handler_new(FlBinaryMessenger* messenger,
                                           FlKeyboardManager* keyboard_manager);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_KEYBOARD_HANDLER_H_
