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

#ifndef APPCODE_VULKAN_VULKAN_BACKBUFFER_H_
#define APPCODE_VULKAN_VULKAN_BACKBUFFER_H_

#include <array>

#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_handle.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "vulkan_command_buffer.h"

namespace vulkan {

class VulkanBackbuffer {
 public:
  VulkanBackbuffer(const VulkanProcTable& vk,
                   const VulkanHandle<VkDevice>& device,
                   const VulkanHandle<VkCommandPool>& pool);

  ~VulkanBackbuffer();

  bool IsValid() const;

  [[nodiscard]] bool WaitFences();

  [[nodiscard]] bool ResetFences();

  const VulkanHandle<VkFence>& GetUsageFence() const;

  const VulkanHandle<VkFence>& GetRenderFence() const;

  const VulkanHandle<VkSemaphore>& GetUsageSemaphore() const;

  const VulkanHandle<VkSemaphore>& GetRenderSemaphore() const;

  VulkanCommandBuffer& GetUsageCommandBuffer();

  VulkanCommandBuffer& GetRenderCommandBuffer();

 private:
  const VulkanProcTable& vk_;
  const VulkanHandle<VkDevice>& device_;
  std::array<VulkanHandle<VkSemaphore>, 2> semaphores_;
  std::array<VulkanHandle<VkFence>, 2> use_fences_;
  VulkanCommandBuffer usage_command_buffer_;
  VulkanCommandBuffer render_command_buffer_;
  bool valid_;

  bool CreateSemaphores();

  bool CreateFences();

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanBackbuffer);
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_BACKBUFFER_H_
