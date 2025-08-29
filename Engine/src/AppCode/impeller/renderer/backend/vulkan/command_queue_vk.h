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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_COMMAND_QUEUE_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_COMMAND_QUEUE_VK_H_

#include "impeller/renderer/command_queue.h"

namespace impeller {

class ContextVK;

class CommandQueueVK : public CommandQueue {
 public:
  explicit CommandQueueVK(const std::weak_ptr<ContextVK>& context);

  ~CommandQueueVK() override;

  fml::Status Submit(
      const std::vector<std::shared_ptr<CommandBuffer>>& buffers,
      const CompletionCallback& completion_callback = {}) override;

 private:
  std::weak_ptr<ContextVK> context_;

  CommandQueueVK(const CommandQueueVK&) = delete;

  CommandQueueVK& operator=(const CommandQueueVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_COMMAND_QUEUE_VK_H_
