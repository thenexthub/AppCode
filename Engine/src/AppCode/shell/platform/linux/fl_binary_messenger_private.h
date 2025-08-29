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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_BINARY_MESSENGER_PRIVATE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_BINARY_MESSENGER_PRIVATE_H_

#include <glib-object.h>

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"

G_BEGIN_DECLS

/**
 * fl_binary_messenger_new:
 * @engine: The #FlEngine to communicate with.
 *
 * Creates a new #FlBinaryMessenger. The binary messenger will take control of
 * the engines platform message handler.
 *
 * Returns: a new #FlBinaryMessenger.
 */
FlBinaryMessenger* fl_binary_messenger_new(FlEngine* engine);

/**
 * fl_binary_messenger_handle_message:
 * @messenger: an #FlBinaryMessenger.
 * @channel: channel message received on.
 * @message: message data.
 * @response_handle: handle to provide to
 * fl_engine_send_platform_message_response().
 *
 * Handles a message received from the engine. Available for testing purposes.
 *
 * Returns: %TRUE if the message is handled.
 */
gboolean fl_binary_messenger_handle_message(
    FlBinaryMessenger* messenger,
    const gchar* channel,
    GBytes* message,
    const FlutterPlatformMessageResponseHandle* response_handle);

/**
 * fl_binary_messenger_shutdown:
 * @messenger: an #FlBinaryMessenger.
 *
 * Shutdown the messenger closing any open channels.
 */
void fl_binary_messenger_shutdown(FlBinaryMessenger* messenger);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_BINARY_MESSENGER_PRIVATE_H_
