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

#ifndef APPCODE_VULKAN_VULKAN_COMMAND_BUFFER_H_
#define APPCODE_VULKAN_VULKAN_COMMAND_BUFFER_H_

#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_handle.h"

namespace vulkan {

class VulkanProcTable;

class VulkanCommandBuffer {
 public:
  VulkanCommandBuffer(const VulkanProcTable& vk,
                      const VulkanHandle<VkDevice>& device,
                      const VulkanHandle<VkCommandPool>& pool);

  ~VulkanCommandBuffer();

  bool IsValid() const;

  VkCommandBuffer Handle() const;

  [[nodiscard]] bool Begin() const;

  [[nodiscard]] bool End() const;

  [[nodiscard]] bool InsertPipelineBarrier(
      VkPipelineStageFlagBits src_stage_flags,
      VkPipelineStageFlagBits dest_stage_flags,
      uint32_t /* mask of VkDependencyFlagBits */ dependency_flags,
      uint32_t memory_barrier_count,
      const VkMemoryBarrier* memory_barriers,
      uint32_t buffer_memory_barrier_count,
      const VkBufferMemoryBarrier* buffer_memory_barriers,
      uint32_t image_memory_barrier_count,
      const VkImageMemoryBarrier* image_memory_barriers) const;

 private:
  const VulkanProcTable& vk_;
  const VulkanHandle<VkDevice>& device_;
  const VulkanHandle<VkCommandPool>& pool_;
  VulkanHandle<VkCommandBuffer> handle_;
  bool valid_;

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanCommandBuffer);
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_COMMAND_BUFFER_H_
