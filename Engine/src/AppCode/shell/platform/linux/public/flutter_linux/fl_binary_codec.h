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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_BINARY_CODEC_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_BINARY_CODEC_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <gmodule.h>

#include "fl_message_codec.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_FINAL_TYPE(FlBinaryCodec,
                     fl_binary_codec,
                     FL,
                     BINARY_CODEC,
                     FlMessageCodec)

/**
 * FlBinaryCodec:
 *
 * #FlBinaryCodec is an #FlMessageCodec that implements the Flutter binary
 * message encoding. This only encodes and decodes #FlValue of type
 * #FL_VALUE_TYPE_UINT8_LIST, other types #FlValues will generate an error
 * during encoding.
 *
 * #FlBinaryCodec matches the BinaryCodec class in the Flutter services
 * library.
 */

/**
 * fl_binary_codec_new:
 *
 * Creates an #FlBinaryCodec.
 *
 * Returns: a new #FlBinaryCodec.
 */
FlBinaryCodec* fl_binary_codec_new();

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_BINARY_CODEC_H_
