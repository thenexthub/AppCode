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

#include "impeller/core/device_buffer.h"

namespace impeller {

DeviceBuffer::DeviceBuffer(DeviceBufferDescriptor desc) : desc_(desc) {}

DeviceBuffer::~DeviceBuffer() = default;

void DeviceBuffer::Flush(std::optional<Range> range) const {}

void DeviceBuffer::Invalidate(std::optional<Range> range) const {}

// static
BufferView DeviceBuffer::AsBufferView(std::shared_ptr<DeviceBuffer> buffer) {
  Range range = {0u, buffer->desc_.size};
  return BufferView(std::move(buffer), range);
}

const DeviceBufferDescriptor& DeviceBuffer::GetDeviceBufferDescriptor() const {
  return desc_;
}

[[nodiscard]] bool DeviceBuffer::CopyHostBuffer(const uint8_t* source,
                                                Range source_range,
                                                size_t offset) {
  if (source_range.length == 0u) {
    // Nothing to copy. Bail.
    return true;
  }

  if (source == nullptr) {
    // Attempted to copy data from a null buffer.
    return false;
  }

  if (desc_.storage_mode != StorageMode::kHostVisible) {
    // One of the storage modes where a transfer queue must be used.
    return false;
  }

  if (offset + source_range.length > desc_.size) {
    // Out of bounds of this buffer.
    return false;
  }

  return OnCopyHostBuffer(source, source_range, offset);
}

}  // namespace impeller
