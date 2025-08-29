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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_STANDARD_METHOD_CODEC_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_STANDARD_METHOD_CODEC_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <gmodule.h>

#include "fl_method_codec.h"
#include "fl_standard_message_codec.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_FINAL_TYPE(FlStandardMethodCodec,
                     fl_standard_method_codec,
                     FL,
                     STANDARD_METHOD_CODEC,
                     FlMethodCodec)

/**
 * FlStandardMethodCodec:
 *
 * #FlStandardMethodCodec is an #FlMethodCodec that implements method calls
 * using the Flutter standard message encoding. It should be used with a
 * #FlMethodChannel.
 *
 * #FlStandardMethodCodec matches the StandardMethodCodec class in the Flutter
 * services library.
 */

/**
 * fl_standard_method_codec_new:
 *
 * Creates an #FlStandardMethodCodec.
 *
 * Returns: a new #FlStandardMethodCodec.
 */
FlStandardMethodCodec* fl_standard_method_codec_new();

/**
 * fl_standard_method_codec_new:
 * @message_codec: A #FlMessageCodec.
 *
 * Creates an #FlStandardMethodCodec with a custom message codec.
 *
 * Returns: a new #FlStandardMethodCodec.
 */
FlStandardMethodCodec* fl_standard_method_codec_new_with_message_codec(
    FlStandardMessageCodec* message_codec);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_STANDARD_METHOD_CODEC_H_
