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

#ifndef APPCODE_IMPELLER_CORE_DEVICE_BUFFER_DESCRIPTOR_H_
#define APPCODE_IMPELLER_CORE_DEVICE_BUFFER_DESCRIPTOR_H_

#include <cstddef>

#include "impeller/core/formats.h"

namespace impeller {

struct DeviceBufferDescriptor {
  StorageMode storage_mode = StorageMode::kDeviceTransient;
  size_t size = 0u;
  // Perhaps we could combine this with storage mode and create appropriate
  // host-write and host-read flags.
  bool readback = false;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_DEVICE_BUFFER_DESCRIPTOR_H_
