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

#include "appcode/vulkan/vulkan_skia_proc_table.h"

namespace vulkan {

skgpu::VulkanGetProc CreateSkiaGetProc(
    const fml::RefPtr<vulkan::VulkanProcTable>& vk) {
  if (!vk || !vk->IsValid()) {
    return nullptr;
  }

  return [vk](const char* proc_name, VkInstance instance, VkDevice device) {
    if (device != VK_NULL_HANDLE) {
      auto result =
          vk->AcquireProc(proc_name, VulkanHandle<VkDevice>{device, nullptr});
      if (result != nullptr) {
        return result;
      }
    }

    return vk->AcquireProc(proc_name,
                           VulkanHandle<VkInstance>{instance, nullptr});
  };
}

}  // namespace vulkan
