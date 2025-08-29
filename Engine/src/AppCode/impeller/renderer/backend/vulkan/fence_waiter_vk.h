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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_FENCE_WAITER_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_FENCE_WAITER_VK_H_

#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>

#include "appcode/fml/closure.h"
#include "impeller/renderer/backend/vulkan/device_holder_vk.h"

namespace impeller {

class ContextVK;
class WaitSetEntry;

using WaitSet = std::vector<std::shared_ptr<WaitSetEntry>>;

class FenceWaiterVK {
 public:
  ~FenceWaiterVK();

  bool IsValid() const;

  void Terminate();

  bool AddFence(vk::UniqueFence fence, const fml::closure& callback);

 private:
  friend class ContextVK;

  std::weak_ptr<DeviceHolderVK> device_holder_;
  std::unique_ptr<std::thread> waiter_thread_;
  std::mutex wait_set_mutex_;
  std::condition_variable wait_set_cv_;
  WaitSet wait_set_;
  bool terminate_ = false;

  explicit FenceWaiterVK(std::weak_ptr<DeviceHolderVK> device_holder);

  void Main();

  bool Wait();
  void WaitUntilEmpty();

  FenceWaiterVK(const FenceWaiterVK&) = delete;

  FenceWaiterVK& operator=(const FenceWaiterVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_FENCE_WAITER_VK_H_
