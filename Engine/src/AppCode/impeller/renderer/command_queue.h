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

#ifndef APPCODE_IMPELLER_RENDERER_COMMAND_QUEUE_H_
#define APPCODE_IMPELLER_RENDERER_COMMAND_QUEUE_H_

#include <functional>

#include "fml/status.h"
#include "impeller/renderer/command_buffer.h"

namespace impeller {

/// @brief An interface for submitting command buffers to the GPU for
///        encoding and execution.
class CommandQueue {
 public:
  using CompletionCallback = std::function<void(CommandBuffer::Status)>;

  CommandQueue();

  virtual ~CommandQueue();

  /// @brief Submit one or more command buffer objects to be encoded and
  ///        executed on the GPU.
  ///
  ///        The order of the provided buffers determines the ordering in which
  ///        they are submitted.
  ///
  ///        The returned status only indicates if the command buffer was
  ///        successfully submitted. Successful completion of the command buffer
  ///        can only be checked in the optional completion callback.
  ///
  ///        Only the Metal and Vulkan backends can give a status beyond
  ///        successful encoding. This callback may be called more than once and
  ///        potentially on a different thread.
  virtual fml::Status Submit(
      const std::vector<std::shared_ptr<CommandBuffer>>& buffers,
      const CompletionCallback& completion_callback = {});

 private:
  CommandQueue(const CommandQueue&) = delete;

  CommandQueue& operator=(const CommandQueue&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_COMMAND_QUEUE_H_
