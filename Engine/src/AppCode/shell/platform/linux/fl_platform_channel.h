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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_PLATFORM_CHANNEL_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_PLATFORM_CHANNEL_H_

#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_call.h"

G_BEGIN_DECLS

typedef enum {
  FL_PLATFORM_CHANNEL_EXIT_TYPE_CANCELABLE,
  FL_PLATFORM_CHANNEL_EXIT_TYPE_REQUIRED,
} FlPlatformChannelExitType;

typedef enum {
  FL_PLATFORM_CHANNEL_EXIT_RESPONSE_CANCEL,
  FL_PLATFORM_CHANNEL_EXIT_RESPONSE_EXIT,
} FlPlatformChannelExitResponse;

G_DECLARE_FINAL_TYPE(FlPlatformChannel,
                     fl_platform_channel,
                     FL,
                     PLATFORM_CHANNEL,
                     GObject);

/**
 * FlPlatformChannel:
 *
 * #FlPlatformChannel is a channel that implements the shell side
 * of SystemChannels.platform from the Flutter services library.
 */

typedef struct {
  FlMethodResponse* (*clipboard_set_data)(FlMethodCall* method_call,
                                          const gchar* text,
                                          gpointer user_data);
  FlMethodResponse* (*clipboard_get_data)(FlMethodCall* method_call,
                                          const gchar* format,
                                          gpointer user_data);
  FlMethodResponse* (*clipboard_has_strings)(FlMethodCall* method_call,
                                             gpointer user_data);
  FlMethodResponse* (*system_exit_application)(FlMethodCall* method_call,
                                               FlPlatformChannelExitType type,
                                               gpointer user_data);
  void (*system_initialization_complete)(gpointer user_data);
  void (*system_sound_play)(const gchar* type, gpointer user_data);
  void (*system_navigator_pop)(gpointer user_data);
} FlPlatformChannelVTable;

/**
 * fl_platform_channel_new:
 * @messenger: an #FlBinaryMessenger
 * @vtable: callbacks for incoming method calls.
 * @user_data: data to pass in callbacks.
 *
 * Creates a new channel that implements SystemChannels.platform from the
 * Flutter services library.
 *
 * Returns: a new #FlPlatformChannel
 */
FlPlatformChannel* fl_platform_channel_new(FlBinaryMessenger* messenger,
                                           FlPlatformChannelVTable* vtable,
                                           gpointer user_data);

/**
 * fl_platform_channel_system_request_app_exit:
 * @channel: an #FlPlatformChannel
 *
 * Request the application exits (i.e. due to the window being requested to be
 * closed).
 *
 * Calling this will only send an exit request to the framework if the framework
 * has already indicated that it is ready to receive requests by sending a
 * "System.initializationComplete" method call on the platform channel. Calls
 * before initialization is complete will result in an immediate exit.
 */
void fl_platform_channel_system_request_app_exit(FlPlatformChannel* channel,
                                                 FlPlatformChannelExitType type,
                                                 GCancellable* cancellable,
                                                 GAsyncReadyCallback callback,
                                                 gpointer user_data);

gboolean fl_platform_channel_system_request_app_exit_finish(
    GObject* object,
    GAsyncResult* result,
    FlPlatformChannelExitResponse* exit_response,
    GError** error);

void fl_platform_channel_respond_system_exit_application(
    FlMethodCall* method_call,
    FlPlatformChannelExitResponse exit_response);

void fl_platform_channel_respond_clipboard_get_data(FlMethodCall* method_call,
                                                    const gchar* text);

void fl_platform_channel_respond_clipboard_has_strings(
    FlMethodCall* method_call,
    gboolean has_strings);

FlMethodResponse* fl_platform_channel_make_system_request_app_exit_response(
    FlPlatformChannelExitResponse exit_response);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_PLATFORM_CHANNEL_H_
