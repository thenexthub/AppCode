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

#include "impeller/renderer/backend/metal/device_buffer_mtl.h"

#include "flutter/fml/logging.h"
#include "impeller/base/validation.h"
#include "impeller/renderer/backend/metal/formats_mtl.h"
#include "impeller/renderer/backend/metal/texture_mtl.h"

namespace impeller {

DeviceBufferMTL::DeviceBufferMTL(DeviceBufferDescriptor desc,
                                 id<MTLBuffer> buffer,
                                 MTLStorageMode storage_mode)
    : DeviceBuffer(desc), buffer_(buffer), storage_mode_(storage_mode) {}

DeviceBufferMTL::~DeviceBufferMTL() = default;

id<MTLBuffer> DeviceBufferMTL::GetMTLBuffer() const {
  return buffer_;
}

uint8_t* DeviceBufferMTL::OnGetContents() const {
#if !FML_OS_IOS
  if (storage_mode_ != MTLStorageModeShared &&
      storage_mode_ != MTLStorageModeManaged) {
    return nullptr;
  }
#else
  if (storage_mode_ != MTLStorageModeShared) {
    return nullptr;
  }
#endif  // !FML_OS_IOS

  return reinterpret_cast<uint8_t*>(buffer_.contents);
}

[[nodiscard]] bool DeviceBufferMTL::OnCopyHostBuffer(const uint8_t* source,
                                                     Range source_range,
                                                     size_t offset) {
  auto dest = static_cast<uint8_t*>(buffer_.contents);

  if (!dest) {
    return false;
  }

  if (source) {
    ::memmove(dest + offset, source + source_range.offset, source_range.length);
  }

// MTLStorageModeManaged is never present on always returns false on iOS. But
// the compiler is mad that `didModifyRange:` appears in a TU meant for iOS. So,
// just compile it away.
#if !FML_OS_IOS
  if (storage_mode_ == MTLStorageModeManaged) {
    [buffer_ didModifyRange:NSMakeRange(offset, source_range.length)];
  }
#endif

  return true;
}

void DeviceBufferMTL::Flush(std::optional<Range> range) const {
#if !FML_OS_IOS
  auto flush_range = range.value_or(Range{0, GetDeviceBufferDescriptor().size});
  if (storage_mode_ == MTLStorageModeManaged) {
    [buffer_
        didModifyRange:NSMakeRange(flush_range.offset, flush_range.length)];
  }
#endif
}

bool DeviceBufferMTL::SetLabel(std::string_view label) {
#ifdef IMPELLER_DEBUG
  if (label.empty()) {
    return false;
  }
  [buffer_ setLabel:@(label.data())];
#endif  // IMPELLER_DEBUG
  return true;
}

bool DeviceBufferMTL::SetLabel(std::string_view label, Range range) {
#ifdef IMPELLER_DEBUG
  if (label.empty()) {
    return false;
  }
  [buffer_ addDebugMarker:@(label.data())
                    range:NSMakeRange(range.offset, range.length)];
#endif  // IMPELLER_DEBUG
  return true;
}

}  // namespace impeller
