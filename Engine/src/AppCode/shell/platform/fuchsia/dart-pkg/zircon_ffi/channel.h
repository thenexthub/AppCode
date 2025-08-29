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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_FFI_CHANNEL_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_FFI_CHANNEL_H_

#include "basic_types.h"
#include "handle.h"
#include "macros.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

ZIRCON_FFI_EXPORT zircon_dart_handle_pair_t* zircon_dart_channel_create(
    uint32_t options);

ZIRCON_FFI_EXPORT int32_t zircon_dart_channel_write(
    zircon_dart_handle_t* handle,
    zircon_dart_byte_array_t* bytes,
    zircon_dart_handle_list_t* handles);

#ifdef __cplusplus
}
#endif

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_FFI_CHANNEL_H_
