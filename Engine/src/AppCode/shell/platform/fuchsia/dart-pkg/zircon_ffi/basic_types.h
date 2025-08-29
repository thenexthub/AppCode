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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_FFI_BASIC_TYPES_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_FFI_BASIC_TYPES_H_

#include "macros.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct zircon_dart_byte_array_t {
  uint8_t* data;
  uint32_t length;
} zircon_dart_byte_array_t;

ZIRCON_FFI_EXPORT zircon_dart_byte_array_t* zircon_dart_byte_array_create(
    uint32_t size);

ZIRCON_FFI_EXPORT void zircon_dart_byte_array_set_value(
    zircon_dart_byte_array_t* arr,
    uint32_t index,
    uint8_t value);

ZIRCON_FFI_EXPORT void zircon_dart_byte_array_free(
    zircon_dart_byte_array_t* arr);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_FFI_BASIC_TYPES_H_
