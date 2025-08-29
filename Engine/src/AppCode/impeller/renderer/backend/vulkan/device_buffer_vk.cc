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

#include "impeller/renderer/backend/vulkan/device_buffer_vk.h"

#include "appcode/appcode_vma/appcode_vma.h"
#include "impeller/renderer/backend/vulkan/context_vk.h"

namespace impeller {

DeviceBufferVK::DeviceBufferVK(DeviceBufferDescriptor desc,
                               std::weak_ptr<Context> context,
                               UniqueBufferVMA buffer,
                               VmaAllocationInfo info,
                               bool is_host_coherent)
    : DeviceBuffer(desc),
      context_(std::move(context)),
      resource_(ContextVK::Cast(*context_.lock().get()).GetResourceManager(),
                BufferResource{
                    std::move(buffer),  //
                    info                //
                }),
      is_host_coherent_(is_host_coherent) {}

DeviceBufferVK::~DeviceBufferVK() = default;

uint8_t* DeviceBufferVK::OnGetContents() const {
  return static_cast<uint8_t*>(resource_->info.pMappedData);
}

bool DeviceBufferVK::OnCopyHostBuffer(const uint8_t* source,
                                      Range source_range,
                                      size_t offset) {
  uint8_t* dest = OnGetContents();

  if (!dest) {
    return false;
  }

  if (source) {
    ::memmove(dest + offset, source + source_range.offset, source_range.length);
  }
  ::vmaFlushAllocation(resource_->buffer.get().allocator,
                       resource_->buffer.get().allocation, offset,
                       source_range.length);

  return true;
}

bool DeviceBufferVK::SetLabel(std::string_view label) {
#ifdef IMPELLER_DEBUG
  auto context = context_.lock();
  if (!context || !resource_->buffer.is_valid()) {
    // The context could have died at this point.
    return false;
  }

  ::vmaSetAllocationName(resource_->buffer.get().allocator,   //
                         resource_->buffer.get().allocation,  //
                         label.data()                         //
  );

  return ContextVK::Cast(*context).SetDebugName(resource_->buffer.get().buffer,
                                                label);
#else
  return true;
#endif  // IMPELLER_DEBUG
}

void DeviceBufferVK::Flush(std::optional<Range> range) const {
  if (is_host_coherent_) {
    return;
  }
  auto flush_range = range.value_or(Range{0, GetDeviceBufferDescriptor().size});
  ::vmaFlushAllocation(resource_->buffer.get().allocator,
                       resource_->buffer.get().allocation, flush_range.offset,
                       flush_range.length);
}

// Visible for testing.
bool DeviceBufferVK::IsHostCoherent() const {
  return is_host_coherent_;
}

void DeviceBufferVK::Invalidate(std::optional<Range> range) const {
  auto flush_range = range.value_or(Range{0, GetDeviceBufferDescriptor().size});
  ::vmaInvalidateAllocation(resource_->buffer.get().allocator,
                            resource_->buffer.get().allocation,
                            flush_range.offset, flush_range.length);
}

bool DeviceBufferVK::SetLabel(std::string_view label, Range range) {
  // We do not have the ability to name ranges. Just name the whole thing.
  return SetLabel(label);
}

vk::Buffer DeviceBufferVK::GetBuffer() const {
  return resource_->buffer.get().buffer;
}

}  // namespace impeller
