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

#include "channel.h"

#include "appcode/fml/logging.h"
#include "appcode/fml/macros.h"

#include <cstdlib>
#include <vector>

#include <lib/zx/channel.h>
#include <zircon/types.h>

static zircon_dart_handle_t* MakeHandle(zx_handle_t handle) {
  zircon_dart_handle_t* result =
      static_cast<zircon_dart_handle_t*>(malloc(sizeof(zircon_dart_handle_t)));
  result->handle = handle;
  return result;
}

zircon_dart_handle_pair_t* zircon_dart_channel_create(uint32_t options) {
  zx_handle_t out0 = 0, out1 = 0;
  zx_status_t status = zx_channel_create(options, &out0, &out1);
  if (status != ZX_OK) {
    return nullptr;
  } else {
    zircon_dart_handle_pair_t* result = static_cast<zircon_dart_handle_pair_t*>(
        malloc(sizeof(zircon_dart_handle_pair_t)));
    result->left = MakeHandle(out0);
    result->right = MakeHandle(out1);
    return result;
  }
}

int32_t zircon_dart_channel_write(zircon_dart_handle_t* handle,
                                  zircon_dart_byte_array_t* bytes,
                                  zircon_dart_handle_list_t* handles) {
  if (!handle || (handle->handle == ZX_HANDLE_INVALID)) {
    return ZX_ERR_BAD_HANDLE;
  }

  std::vector<zx_handle_t> zx_handles;
  std::vector<zircon_dart_handle_t*> handle_data =
      *reinterpret_cast<std::vector<zircon_dart_handle_t*>*>(handles->data);
  for (auto handle_ptr : handle_data) {
    zx_handles.push_back(handle_ptr->handle);
  }

  zx_status_t status =
      zx_channel_write(handle->handle, 0, bytes->data, bytes->length,
                       zx_handles.data(), zx_handles.size());

  // Handles are always consumed.
  for (auto handle_ptr : handle_data) {
    handle_ptr->handle = ZX_HANDLE_INVALID;
  }

  return status;
}
