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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_STRING_CODEC_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_STRING_CODEC_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <gmodule.h>

#include "fl_message_codec.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_FINAL_TYPE(FlStringCodec,
                     fl_string_codec,
                     FL,
                     STRING_CODEC,
                     FlMessageCodec)

/**
 * FlStringCodec:
 *
 * #FlStringCodec is an #FlMessageCodec that implements the Flutter string
 * message encoding. This only encodes and decodes #FlValue of type
 * #FL_VALUE_TYPE_STRING, other types #FlValues will generate an error during
 * encoding.
 *
 * #FlStringCodec matches the StringCodec class in the Flutter services library.
 */

/**
 * fl_string_codec_new:
 *
 * Creates an #FlStringCodec.
 *
 * Returns: a new #FlStringCodec.
 */
FlStringCodec* fl_string_codec_new();

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_STRING_CODEC_H_
