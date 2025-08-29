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

#include "appcode/shell/platform/linux/fl_keyboard_channel.h"

#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_channel.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_standard_method_codec.h"

static constexpr char kChannelName[] = "appcode/keyboard";

static constexpr char kGetKeyboardStateMethod[] = "getKeyboardState";

struct _FlKeyboardChannel {
  GObject parent_instance;

  FlMethodChannel* channel;

  FlKeyboardChannelVTable* vtable;

  gpointer user_data;
};

G_DEFINE_TYPE(FlKeyboardChannel, fl_keyboard_channel, G_TYPE_OBJECT)

static FlMethodResponse* get_keyboard_state(FlKeyboardChannel* self) {
  g_autoptr(FlValue) result = self->vtable->get_keyboard_state(self->user_data);
  return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}

// Called when a method call is received from appcode.
static void method_call_cb(FlMethodChannel* channel,
                           FlMethodCall* method_call,
                           gpointer user_data) {
  FlKeyboardChannel* self = FL_KEYBOARD_CHANNEL(user_data);

  const gchar* method = fl_method_call_get_name(method_call);
  g_autoptr(FlMethodResponse) response = nullptr;
  if (strcmp(method, kGetKeyboardStateMethod) == 0) {
    response = get_keyboard_state(self);
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  g_autoptr(GError) error = nullptr;
  if (!fl_method_call_respond(method_call, response, &error)) {
    g_warning("Failed to send method call response: %s", error->message);
  }
}

static void fl_keyboard_channel_dispose(GObject* object) {
  FlKeyboardChannel* self = FL_KEYBOARD_CHANNEL(object);

  g_clear_object(&self->channel);

  G_OBJECT_CLASS(fl_keyboard_channel_parent_class)->dispose(object);
}

static void fl_keyboard_channel_class_init(FlKeyboardChannelClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = fl_keyboard_channel_dispose;
}

static void fl_keyboard_channel_init(FlKeyboardChannel* self) {}

FlKeyboardChannel* fl_keyboard_channel_new(FlBinaryMessenger* messenger,
                                           FlKeyboardChannelVTable* vtable,
                                           gpointer user_data) {
  g_return_val_if_fail(FL_IS_BINARY_MESSENGER(messenger), nullptr);
  g_return_val_if_fail(vtable != nullptr, nullptr);

  FlKeyboardChannel* self = FL_KEYBOARD_CHANNEL(
      g_object_new(fl_keyboard_channel_get_type(), nullptr));

  self->vtable = vtable;
  self->user_data = user_data;

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  self->channel =
      fl_method_channel_new(messenger, kChannelName, FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(self->channel, method_call_cb, self,
                                            nullptr);

  return self;
}
