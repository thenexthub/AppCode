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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_PLATFORM_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_PLATFORM_HANDLER_H_

#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlPlatformHandler,
                     fl_platform_handler,
                     FL,
                     PLATFORM_HANDLER,
                     GObject);

/**
 * FlPlatformHandler:
 *
 * #FlPlatformHandler is a handler that implements the shell side
 * of SystemChannels.platform from the Flutter services library.
 */

/**
 * fl_platform_handler_new:
 * @messenger: an #FlBinaryMessenger
 *
 * Creates a new handler that implements SystemChannels.platform from the
 * Flutter services library.
 *
 * Returns: a new #FlPlatformHandler
 */
FlPlatformHandler* fl_platform_handler_new(FlBinaryMessenger* messenger);

/**
 * fl_platform_handler_request_app_exit:
 * @handler: an #FlPlatformHandler
 *
 * Request the application exits (i.e. due to the window being requested to be
 * closed).
 *
 * Calling this will only send an exit request to the framework if the framework
 * has already indicated that it is ready to receive requests by sending a
 * "System.initializationComplete" method call on the platform channel. Calls
 * before initialization is complete will result in an immediate exit.
 */
void fl_platform_handler_request_app_exit(FlPlatformHandler* handler);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_PLATFORM_HANDLER_H_
