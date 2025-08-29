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

#include "impeller/core/buffer_view.h"

namespace impeller {

BufferView::BufferView() : buffer_(nullptr), raw_buffer_(nullptr), range_({}) {}

BufferView::BufferView(DeviceBuffer* buffer, Range range)
    : buffer_(), raw_buffer_(buffer), range_(range) {}

BufferView::BufferView(std::shared_ptr<const DeviceBuffer> buffer, Range range)
    : buffer_(std::move(buffer)), raw_buffer_(nullptr), range_(range) {}

const DeviceBuffer* BufferView::GetBuffer() const {
  return raw_buffer_ ? raw_buffer_ : buffer_.get();
}

std::shared_ptr<const DeviceBuffer> BufferView::TakeBuffer() {
  if (buffer_) {
    raw_buffer_ = buffer_.get();
    return std::move(buffer_);
  } else {
    return nullptr;
  }
}

BufferView::operator bool() const {
  return buffer_ || raw_buffer_;
}

}  // namespace impeller
