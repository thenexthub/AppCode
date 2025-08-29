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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_JSON_MESSAGE_CODEC_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_JSON_MESSAGE_CODEC_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <gmodule.h>

#include "fl_message_codec.h"

G_BEGIN_DECLS

/**
 * FlJsonMessageCodecError:
 * @FL_JSON_MESSAGE_CODEC_ERROR_INVALID_UTF8: Message is not valid UTF-8.
 * @FL_JSON_MESSAGE_CODEC_ERROR_INVALID_JSON: Message is not valid JSON.
 * @FL_JSON_MESSAGE_CODEC_ERROR_INVALID_OBJECT_KEY_TYPE: Invalid object key
 * type.
 *
 * Errors for #FlJsonMessageCodec objects to set on failures.
 */
#define FL_JSON_MESSAGE_CODEC_ERROR fl_json_message_codec_error_quark()

typedef enum {
  FL_JSON_MESSAGE_CODEC_ERROR_INVALID_UTF8,
  FL_JSON_MESSAGE_CODEC_ERROR_INVALID_JSON,
  FL_JSON_MESSAGE_CODEC_ERROR_INVALID_OBJECT_KEY_TYPE,
} FlJsonMessageCodecError;

G_MODULE_EXPORT
GQuark fl_json_message_codec_error_quark(void) G_GNUC_CONST;

G_MODULE_EXPORT
G_DECLARE_FINAL_TYPE(FlJsonMessageCodec,
                     fl_json_message_codec,
                     FL,
                     JSON_CODEC,
                     FlMessageCodec)

/**
 * FlJsonMessageCodec:
 *
 * #FlJsonMessageCodec is an #FlMessageCodec that implements the encodes
 * #FlValue to/from JSON. This codec encodes and decodes #FlValue of type
 * #FL_VALUE_TYPE_NULL, #FL_VALUE_TYPE_BOOL, #FL_VALUE_TYPE_INT,
 * #FL_VALUE_TYPE_FLOAT, #FL_VALUE_TYPE_STRING, #FL_VALUE_TYPE_UINT8_LIST,
 * #FL_VALUE_TYPE_INT32_LIST, #FL_VALUE_TYPE_INT64_LIST,
 * #FL_VALUE_TYPE_FLOAT_LIST, #FL_VALUE_TYPE_LIST, and #FL_VALUE_TYPE_MAP.
 *
 * #FlJsonMessageCodec matches the JSONMessageCodec class in the Flutter
 * services library.
 */

/**
 * fl_json_message_codec_new:
 *
 * Creates an #FlJsonMessageCodec.
 *
 * Returns: a new #FlJsonMessageCodec.
 */
FlJsonMessageCodec* fl_json_message_codec_new();

/**
 * fl_json_message_codec_encode:
 * @codec: an #FlJsonMessageCodec.
 * @value: value to encode.
 * @error: (allow-none): #GError location to store the error occurring, or
 * %NULL.
 *
 * Encodes a value to a JSON string.
 *
 * Returns: a JSON representation of this value or %NULL on error.
 */
gchar* fl_json_message_codec_encode(FlJsonMessageCodec* codec,
                                    FlValue* value,
                                    GError** error);

/**
 * fl_json_message_codec_decode:
 * @codec: an #FlJsonMessageCodec.
 * @text: UTF-8 text in JSON format.
 * @error: (allow-none): #GError location to store the error occurring, or
 * %NULL.
 *
 * Decodes a value from a JSON string.
 *
 * Returns: an #FlValue or %NULL on error.
 */
FlValue* fl_json_message_codec_decode(FlJsonMessageCodec* codec,
                                      const gchar* text,
                                      GError** error);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_JSON_MESSAGE_CODEC_H_
