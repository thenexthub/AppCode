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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_KEYBOARD_CHANNEL_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_KEYBOARD_CHANNEL_H_

#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlKeyboardChannel,
                     fl_keyboard_channel,
                     FL,
                     KEYBOARD_CHANNEL,
                     GObject);

/**
 * FlKeyboardChannel:
 *
 * #FlKeyboardChannel is a channel that implements the shell side
 * of SystemChannels.keyboard from the Flutter services library.
 */

typedef struct {
  FlValue* (*get_keyboard_state)(gpointer user_data);
} FlKeyboardChannelVTable;

/**
 * fl_keyboard_channel_new:
 * @messenger: an #FlBinaryMessenger
 * @vtable: callbacks for incoming method calls.
 * @user_data: data to pass in callbacks.
 *
 * Creates a new channel that implements SystemChannels.keyboard from the
 * Flutter services library.
 *
 * Returns: a new #FlKeyboardChannel
 */
FlKeyboardChannel* fl_keyboard_channel_new(FlBinaryMessenger* messenger,
                                           FlKeyboardChannelVTable* vtable,
                                           gpointer user_data);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_KEYBOARD_CHANNEL_H_
