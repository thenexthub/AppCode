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

#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_codec.h"
#include "appcode/shell/platform/linux/fl_method_codec_private.h"

#include <gmodule.h>

G_DEFINE_TYPE(FlMethodCodec, fl_method_codec, G_TYPE_OBJECT)

static void fl_method_codec_class_init(FlMethodCodecClass* klass) {}

static void fl_method_codec_init(FlMethodCodec* self) {}

GBytes* fl_method_codec_encode_method_call(FlMethodCodec* self,
                                           const gchar* name,
                                           FlValue* args,
                                           GError** error) {
  g_return_val_if_fail(FL_IS_METHOD_CODEC(self), nullptr);
  g_return_val_if_fail(name != nullptr, nullptr);

  return FL_METHOD_CODEC_GET_CLASS(self)->encode_method_call(self, name, args,
                                                             error);
}

gboolean fl_method_codec_decode_method_call(FlMethodCodec* self,
                                            GBytes* message,
                                            gchar** name,
                                            FlValue** args,
                                            GError** error) {
  g_return_val_if_fail(FL_IS_METHOD_CODEC(self), FALSE);
  g_return_val_if_fail(message != nullptr, FALSE);
  g_return_val_if_fail(name != nullptr, FALSE);
  g_return_val_if_fail(args != nullptr, FALSE);

  return FL_METHOD_CODEC_GET_CLASS(self)->decode_method_call(self, message,
                                                             name, args, error);
}

GBytes* fl_method_codec_encode_success_envelope(FlMethodCodec* self,
                                                FlValue* result,
                                                GError** error) {
  g_return_val_if_fail(FL_IS_METHOD_CODEC(self), nullptr);

  return FL_METHOD_CODEC_GET_CLASS(self)->encode_success_envelope(self, result,
                                                                  error);
}

GBytes* fl_method_codec_encode_error_envelope(FlMethodCodec* self,
                                              const gchar* code,
                                              const gchar* message,
                                              FlValue* details,
                                              GError** error) {
  g_return_val_if_fail(FL_IS_METHOD_CODEC(self), nullptr);
  g_return_val_if_fail(code != nullptr, nullptr);

  return FL_METHOD_CODEC_GET_CLASS(self)->encode_error_envelope(
      self, code, message, details, error);
}

GBytes* fl_method_codec_encode_response(FlMethodCodec* self,
                                        FlMethodResponse* response,
                                        GError** error) {
  g_return_val_if_fail(FL_IS_METHOD_CODEC(self), nullptr);
  g_return_val_if_fail(FL_IS_METHOD_SUCCESS_RESPONSE(response) ||
                           FL_IS_METHOD_ERROR_RESPONSE(response) ||
                           FL_IS_METHOD_NOT_IMPLEMENTED_RESPONSE(response),
                       nullptr);

  if (FL_IS_METHOD_SUCCESS_RESPONSE(response)) {
    FlMethodSuccessResponse* r = FL_METHOD_SUCCESS_RESPONSE(response);
    return fl_method_codec_encode_success_envelope(
        self, fl_method_success_response_get_result(r), error);
  } else if (FL_IS_METHOD_ERROR_RESPONSE(response)) {
    FlMethodErrorResponse* r = FL_METHOD_ERROR_RESPONSE(response);
    return fl_method_codec_encode_error_envelope(
        self, fl_method_error_response_get_code(r),
        fl_method_error_response_get_message(r),
        fl_method_error_response_get_details(r), error);
  } else if (FL_IS_METHOD_NOT_IMPLEMENTED_RESPONSE(response)) {
    return g_bytes_new(nullptr, 0);
  } else {
    g_assert_not_reached();
  }
}

FlMethodResponse* fl_method_codec_decode_response(FlMethodCodec* self,
                                                  GBytes* message,
                                                  GError** error) {
  g_return_val_if_fail(FL_IS_METHOD_CODEC(self), nullptr);
  g_return_val_if_fail(message != nullptr, nullptr);

  if (g_bytes_get_size(message) == 0) {
    return FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  return FL_METHOD_CODEC_GET_CLASS(self)->decode_response(self, message, error);
}
