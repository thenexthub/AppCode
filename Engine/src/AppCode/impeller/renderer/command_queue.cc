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

#include "impeller/renderer/command_queue.h"
#include "impeller/renderer/command_buffer.h"

namespace impeller {

CommandQueue::CommandQueue() = default;

CommandQueue::~CommandQueue() = default;

fml::Status CommandQueue::Submit(
    const std::vector<std::shared_ptr<CommandBuffer>>& buffers,
    const CompletionCallback& completion_callback) {
  if (buffers.empty()) {
    if (completion_callback) {
      completion_callback(CommandBuffer::Status::kError);
    }
    return fml::Status(fml::StatusCode::kInvalidArgument,
                       "No command buffers provided.");
  }
  for (const std::shared_ptr<CommandBuffer>& buffer : buffers) {
    if (!buffer->SubmitCommands(completion_callback)) {
      return fml::Status(fml::StatusCode::kCancelled,
                         "Failed to submit command buffer.");
    }
  }
  return fml::Status();
}

}  // namespace impeller
