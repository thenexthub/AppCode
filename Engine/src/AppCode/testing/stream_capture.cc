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

#include "appcode/testing/stream_capture.h"

namespace appcode::testing {

StreamCapture::StreamCapture(std::ostream* ostream)
    : ostream_(ostream), old_buffer_(ostream_->rdbuf()) {
  ostream_->rdbuf(buffer_.rdbuf());
}

StreamCapture::~StreamCapture() {
  Stop();
}

void StreamCapture::Stop() {
  if (old_buffer_) {
    ostream_->rdbuf(old_buffer_);
    old_buffer_ = nullptr;
  }
}

std::string StreamCapture::GetOutput() const {
  return buffer_.str();
}

}  // namespace appcode::testing
