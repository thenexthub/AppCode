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

#include "vulkan_provider.h"

namespace vulkan {

vulkan::VulkanHandle<VkFence> VulkanProvider::CreateFence() {
  const VkFenceCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };
  VkFence fence;
  if (VK_CALL_LOG_ERROR(vk().CreateFence(vk_device(), &create_info, nullptr,
                                         &fence)) != VK_SUCCESS)
    return vulkan::VulkanHandle<VkFence>();

  return {fence, [this](VkFence fence) {
            vk().DestroyFence(vk_device(), fence, nullptr);
          }};
}

}  // namespace vulkan
