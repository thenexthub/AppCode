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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_METHOD_CALL_PRIVATE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_METHOD_CALL_PRIVATE_H_

#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_call.h"

#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_channel.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_value.h"

G_BEGIN_DECLS

/**
 * fl_method_call_new:
 * @name: a method name.
 * @args: arguments provided to a method.
 * @channel: channel call received on.
 * @response_handle: handle to respond with.
 *
 * Creates a method call.
 *
 * Returns: a new #FlMethodCall.
 */
FlMethodCall* fl_method_call_new(
    const gchar* name,
    FlValue* args,
    FlMethodChannel* channel,
    FlBinaryMessengerResponseHandle* response_handle);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_METHOD_CALL_PRIVATE_H_
