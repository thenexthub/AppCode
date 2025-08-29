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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_METHOD_CODEC_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_METHOD_CODEC_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <glib-object.h>
#include <gmodule.h>

#include "fl_method_response.h"
#include "fl_value.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_DERIVABLE_TYPE(FlMethodCodec,
                         fl_method_codec,
                         FL,
                         METHOD_CODEC,
                         GObject)

/**
 * FlMethodCodec:
 *
 * #FlMethodCodec is an abstract class that encodes and decodes method calls on
 * a platform channel. Override this class to implement an encoding.
 *
 * #FlMethodCodec matches the MethodCodec class in the Flutter services
 * library.
 */

struct _FlMethodCodecClass {
  GObjectClass parent_class;

  /**
   * FlMethodCodec::encode_method_call:
   * @codec: an #FlMethodCodec.
   * @name: method name.
   * @args: (allow-none): method arguments, or %NULL.
   * @error: (allow-none): #GError location to store the error occurring, or
   * %NULL.
   *
   * Encodes a method call.
   *
   * Returns: (transfer full): a binary encoding of this method call or %NULL if
   * not able to encode.
   */
  GBytes* (*encode_method_call)(FlMethodCodec* codec,
                                const gchar* name,
                                FlValue* args,
                                GError** error);

  /**
   * FlMethodCodec::decode_method_call:
   * @codec: an #FlMethodCodec
   * @message: message to decode.
   * @name: (transfer full): location to write method name or %NULL if not
   * required
   * @args: (transfer full): location to write method arguments, or %NULL if not
   * required
   * @error: (allow-none): #GError location to store the error occurring, or
   * %NULL
   *
   * Decodes a method call.
   *
   * Returns: %TRUE if successfully decoded.
   */
  gboolean (*decode_method_call)(FlMethodCodec* codec,
                                 GBytes* message,
                                 gchar** name,
                                 FlValue** args,
                                 GError** error);

  /**
   * FlMethodCodec::encode_success_envelope:
   * @codec: an #FlMethodCodec.
   * @result: (allow-none): method result, or %NULL.
   * @error: (allow-none): #GError location to store the error occurring, or
   * %NULL.
   *
   * Encodes a successful response to a method call.
   *
   * Returns: (transfer full): a binary encoding of this response or %NULL if
   * not able to encode.
   */
  GBytes* (*encode_success_envelope)(FlMethodCodec* codec,
                                     FlValue* result,
                                     GError** error);

  /**
   * FlMethodCodec::encode_error_envelope:
   * @codec: an #FlMethodCodec.
   * @code: an error code.
   * @message: (allow-none): an error message, or %NULL.
   * @details: (allow-none): error details, or %NULL.
   * @error: (allow-none): #GError location to store the error occurring, or
   * %NULL.
   *
   * Encodes an error response to a method call.
   *
   * Returns: (transfer full): a binary encoding of this response or %NULL if
   * not able to encode.
   */
  GBytes* (*encode_error_envelope)(FlMethodCodec* codec,
                                   const gchar* code,
                                   const gchar* message,
                                   FlValue* details,
                                   GError** error);

  /**
   * FlMethodCodec::decode_response:
   * @codec: an #FlMethodCodec.
   * @message: message to decode.
   * @error: (allow-none): #GError location to store the error occurring, or
   * %NULL.
   *
   * Decodes a response to a method call.
   *
   * Returns: a new #FlMethodResponse or %NULL on error.
   */
  FlMethodResponse* (*decode_response)(FlMethodCodec* codec,
                                       GBytes* message,
                                       GError** error);
};

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_METHOD_CODEC_H_
