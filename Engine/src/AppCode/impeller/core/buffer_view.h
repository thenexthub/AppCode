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

#ifndef APPCODE_IMPELLER_CORE_BUFFER_VIEW_H_
#define APPCODE_IMPELLER_CORE_BUFFER_VIEW_H_

#include <memory>
#include "impeller/core/range.h"

namespace impeller {

class DeviceBuffer;

/// A specific range in a DeviceBuffer.
///
/// BufferView can maintain ownership over the DeviceBuffer or not depending on
/// if it is created with a std::shared_ptr or a raw pointer.
struct BufferView {
 public:
  BufferView();

  BufferView(DeviceBuffer* buffer, Range range);

  BufferView(std::shared_ptr<const DeviceBuffer> buffer, Range range);

  Range GetRange() const { return range_; }

  const DeviceBuffer* GetBuffer() const;

  std::shared_ptr<const DeviceBuffer> TakeBuffer();

  explicit operator bool() const;

 private:
  std::shared_ptr<const DeviceBuffer> buffer_;
  /// This is a non-owned DeviceBuffer. Steps should be taken to make sure this
  /// lives for the duration of the BufferView's life. Usually this is done
  /// automatically by the graphics API or in the case of Vulkan the HostBuffer
  /// or TrackedObjectsVK keeps it alive.
  const DeviceBuffer* raw_buffer_;
  Range range_;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_BUFFER_VIEW_H_
