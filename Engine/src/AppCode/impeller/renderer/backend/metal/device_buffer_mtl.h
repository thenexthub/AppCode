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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_DEVICE_BUFFER_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_DEVICE_BUFFER_MTL_H_

#include <Metal/Metal.h>

#include "impeller/base/backend_cast.h"
#include "impeller/core/device_buffer.h"

namespace impeller {

class DeviceBufferMTL final
    : public DeviceBuffer,
      public BackendCast<DeviceBufferMTL, DeviceBuffer> {
 public:
  DeviceBufferMTL();

  // |DeviceBuffer|
  ~DeviceBufferMTL() override;

  id<MTLBuffer> GetMTLBuffer() const;

 private:
  friend class AllocatorMTL;

  const id<MTLBuffer> buffer_;
  const MTLStorageMode storage_mode_;

  DeviceBufferMTL(DeviceBufferDescriptor desc,
                  id<MTLBuffer> buffer,
                  MTLStorageMode storage_mode);

  // |DeviceBuffer|
  uint8_t* OnGetContents() const override;

  // |DeviceBuffer|
  bool OnCopyHostBuffer(const uint8_t* source,
                        Range source_range,
                        size_t offset) override;

  // |DeviceBuffer|
  bool SetLabel(std::string_view label) override;

  // |DeviceBuffer|
  bool SetLabel(std::string_view label, Range range) override;

  // |DeviceBuffer|
  void Flush(std::optional<Range> range) const override;

  DeviceBufferMTL(const DeviceBufferMTL&) = delete;

  DeviceBufferMTL& operator=(const DeviceBufferMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_DEVICE_BUFFER_MTL_H_
