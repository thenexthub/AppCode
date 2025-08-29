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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_METHOD_CHANNEL_PRIVATE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_METHOD_CHANNEL_PRIVATE_H_

#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_channel.h"

#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_response.h"

G_BEGIN_DECLS

/**
 * fl_method_channel_respond:
 * @channel: an #FlMethodChannel.
 * @response_handle: an #FlBinaryMessengerResponseHandle.
 * @response: an #FlMethodResponse.
 * @error: (allow-none): #GError location to store the error occurring, or %NULL
 * to ignore.
 *
 * Responds to a method call.
 *
 * Returns: %TRUE on success.
 */
gboolean fl_method_channel_respond(
    FlMethodChannel* channel,
    FlBinaryMessengerResponseHandle* response_handle,
    FlMethodResponse* response,
    GError** error);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_METHOD_CHANNEL_PRIVATE_H_
