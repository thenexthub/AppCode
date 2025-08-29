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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_WINDOWING_CHANNEL_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_WINDOWING_CHANNEL_H_

#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_response.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlWindowingChannel,
                     fl_windowing_channel,
                     FL,
                     WINDOWING_CHANNEL,
                     GObject);

// States that a window can be in.
typedef enum {
  FL_WINDOW_STATE_UNDEFINED,
  FL_WINDOW_STATE_RESTORED,
  FL_WINDOW_STATE_MAXIMIZED,
  FL_WINDOW_STATE_MINIMIZED
} FlWindowState;

// Size dimensions used in windowing channel.
typedef struct {
  double width;
  double height;
} FlWindowingSize;

/**
 * FlWindowingChannel:
 *
 * #FlWindowingChannel is a channel that implements the shell side
 * of SystemChannels.windowing from the Flutter services library.
 */

typedef struct {
  FlMethodResponse* (*create_regular)(FlWindowingSize* size,
                                      FlWindowingSize* min_size,
                                      FlWindowingSize* max_size,
                                      const gchar* title,
                                      FlWindowState state,
                                      gpointer user_data);
  FlMethodResponse* (*modify_regular)(int64_t view_id,
                                      FlWindowingSize* size,
                                      const gchar* title,
                                      FlWindowState state,
                                      gpointer user_data);
  FlMethodResponse* (*destroy_window)(int64_t view_id, gpointer user_data);
} FlWindowingChannelVTable;

/**
 * fl_windowing_channel_new:
 * @messenger: an #FlBinaryMessenger
 * @vtable: callbacks for incoming method calls.
 * @user_data: data to pass in callbacks.
 *
 * Creates a new channel that sends handled windowing requests from the
 * platform.
 *
 * Returns: a new #FlWindowingChannel
 */
FlWindowingChannel* fl_windowing_channel_new(FlBinaryMessenger* messenger,
                                             FlWindowingChannelVTable* vtable,
                                             gpointer user_data);

FlMethodResponse* fl_windowing_channel_make_create_regular_response(
    int64_t view_id,
    FlWindowingSize* size,
    FlWindowState state);

FlMethodResponse* fl_windowing_channel_make_modify_regular_response();

FlMethodResponse* fl_windowing_channel_make_destroy_window_response();

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_WINDOWING_CHANNEL_H_
