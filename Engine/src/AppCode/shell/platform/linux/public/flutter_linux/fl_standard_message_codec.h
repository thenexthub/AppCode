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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_STANDARD_MESSAGE_CODEC_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_STANDARD_MESSAGE_CODEC_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <gmodule.h>

#include "fl_message_codec.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_DERIVABLE_TYPE(FlStandardMessageCodec,
                         fl_standard_message_codec,
                         FL,
                         STANDARD_MESSAGE_CODEC,
                         FlMessageCodec)

/**
 * FlStandardMessageCodec:
 *
 * #FlStandardMessageCodec is an #FlMessageCodec that implements the Flutter
 * standard message encoding. This codec encodes and decodes #FlValue of type
 * #FL_VALUE_TYPE_NULL, #FL_VALUE_TYPE_BOOL, #FL_VALUE_TYPE_INT,
 * #FL_VALUE_TYPE_FLOAT, #FL_VALUE_TYPE_STRING, #FL_VALUE_TYPE_UINT8_LIST,
 * #FL_VALUE_TYPE_INT32_LIST, #FL_VALUE_TYPE_INT64_LIST,
 * #FL_VALUE_TYPE_FLOAT_LIST, #FL_VALUE_TYPE_LIST, and #FL_VALUE_TYPE_MAP.
 *
 * If other values types are required to be supported create a new subclass that
 * overrides write_value and read_value_of_type.
 *
 * #FlStandardMessageCodec matches the StandardCodec class in the Flutter
 * services library.
 */

struct _FlStandardMessageCodecClass {
  FlMessageCodecClass parent_class;

  /**
   * FlStandardMessageCodec::write_value:
   * @codec: an #FlStandardMessageCodec.
   * @buffer: a buffer to write into.
   * @value: (allow-none): value to write.
   * @error: (allow-none): #GError location to store the error occurring, or
   * %NULL.
   *
   * Virtual method to write an #FlValue in Flutter Standard encoding.
   *
   * If a codec needs to support custom #FlValue objects it must override this
   * method to encode those values. For non-custom values the parent method
   * should be called.
   *
   * Returns: %TRUE on success.
   */
  gboolean (*write_value)(FlStandardMessageCodec* codec,
                          GByteArray* buffer,
                          FlValue* value,
                          GError** error);

  /**
   * FlStandardMessageCodec::read_value_of_type:
   * @codec: an #FlStandardMessageCodec.
   * @buffer: buffer to read from.
   * @offset: (inout): read position in @buffer.
   * @type: the type of the value.
   * @error: (allow-none): #GError location to store the error occurring, or
   * %NULL.
   *
   * Virtual method to read an #FlValue in Flutter Standard encoding.
   *
   * If a codec needs to support custom #FlValue objects it must override this
   * method to decode those values. For non-custom values the parent method
   * should be called.
   *
   * Returns: an #FlValue or %NULL on error.
   */
  FlValue* (*read_value_of_type)(FlStandardMessageCodec* codec,
                                 GBytes* buffer,
                                 size_t* offset,
                                 int type,
                                 GError** error);
};

/*
 * fl_standard_message_codec_new:
 *
 * Creates an #FlStandardMessageCodec.
 *
 * Returns: a new #FlStandardMessageCodec.
 */
FlStandardMessageCodec* fl_standard_message_codec_new();

/**
 * fl_standard_message_codec_write_size:
 * @codec: an #FlStandardMessageCodec.
 * @buffer: buffer to write into.
 * @size: size value to write.
 *
 * Writes a size field in Flutter Standard encoding.
 */
void fl_standard_message_codec_write_size(FlStandardMessageCodec* codec,
                                          GByteArray* buffer,
                                          uint32_t size);

/**
 * fl_standard_message_codec_read_size:
 * @codec: an #FlStandardMessageCodec.
 * @buffer: buffer to read from.
 * @offset: (inout): read position in @buffer.
 * @value: location to read size.
 * @error: (allow-none): #GError location to store the error occurring, or
 * %NULL.
 *
 * Reads a size field in Flutter Standard encoding.
 *
 * This method is intended for use by subclasses overriding
 * FlStandardMessageCodec::read_value_of_type.
 *
 * Returns: %TRUE on success.
 */
gboolean fl_standard_message_codec_read_size(FlStandardMessageCodec* codec,
                                             GBytes* buffer,
                                             size_t* offset,
                                             uint32_t* value,
                                             GError** error);

/**
 * fl_standard_message_codec_write_value:
 * @codec: an #FlStandardMessageCodec.
 * @buffer: buffer to write into.
 * @value: (allow-none): value to write.
 * @error: (allow-none): #GError location to store the error occurring, or
 * %NULL.
 *
 * Writes an #FlValue in Flutter Standard encoding.
 *
 * This method is intended for use by subclasses overriding
 * FlStandardMessageCodec::write_value.
 *
 * Returns: %TRUE on success.
 */
gboolean fl_standard_message_codec_write_value(FlStandardMessageCodec* codec,
                                               GByteArray* buffer,
                                               FlValue* value,
                                               GError** error);

/**
 * fl_standard_message_codec_read_value:
 * @codec: an #FlStandardMessageCodec.
 * @buffer: buffer to read from.
 * @offset: (inout): read position in @buffer.
 * @value: location to read size.
 * @error: (allow-none): #GError location to store the error occurring, or
 * %NULL.
 *
 * Reads an #FlValue in Flutter Standard encoding.
 *
 * This method is intended for use by subclasses overriding
 * FlStandardMessageCodec::read_value_of_type.
 *
 * Returns: a new #FlValue or %NULL on error.
 */
FlValue* fl_standard_message_codec_read_value(FlStandardMessageCodec* codec,
                                              GBytes* buffer,
                                              size_t* offset,
                                              GError** error);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_STANDARD_MESSAGE_CODEC_H_
