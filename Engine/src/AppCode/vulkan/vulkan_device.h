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

#ifndef APPCODE_VULKAN_VULKAN_DEVICE_H_
#define APPCODE_VULKAN_VULKAN_DEVICE_H_

#include <vector>

#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_handle.h"

namespace vulkan {

class VulkanProcTable;
class VulkanSurface;

class VulkanDevice {
 public:
  /// @brief  Create a new VkDevice with a resolved VkQueue suitable for
  ///         rendering with Skia.
  ///
  VulkanDevice(VulkanProcTable& vk,
               VulkanHandle<VkPhysicalDevice> physical_device,
               bool enable_validation_layers);

  /// @brief  Wrap an existing VkDevice and VkQueue.
  ///
  VulkanDevice(VulkanProcTable& vk,
               VulkanHandle<VkPhysicalDevice> physical_device,
               VulkanHandle<VkDevice> device,
               uint32_t queue_family_index,
               VulkanHandle<VkQueue> queue);
  ~VulkanDevice();

  bool IsValid() const;

  const VulkanHandle<VkDevice>& GetHandle() const;

  const VulkanHandle<VkPhysicalDevice>& GetPhysicalDeviceHandle() const;

  const VulkanHandle<VkQueue>& GetQueueHandle() const;

  const VulkanHandle<VkCommandPool>& GetCommandPool() const;

  uint32_t GetGraphicsQueueIndex() const;

  void ReleaseDeviceOwnership();

  [[nodiscard]] bool GetSurfaceCapabilities(
      const VulkanSurface& surface,
      VkSurfaceCapabilitiesKHR* capabilities) const;

  [[nodiscard]] bool GetPhysicalDeviceFeatures(
      VkPhysicalDeviceFeatures* features) const;

  [[nodiscard]] int ChooseSurfaceFormat(
      const VulkanSurface& surface,
      const std::vector<VkFormat>& desired_formats,
      VkSurfaceFormatKHR* format) const;

  [[nodiscard]] bool ChoosePresentMode(const VulkanSurface& surface,
                                       VkPresentModeKHR* present_mode) const;

  [[nodiscard]] bool QueueSubmit(
      std::vector<VkPipelineStageFlags> wait_dest_pipeline_stages,
      const std::vector<VkSemaphore>& wait_semaphores,
      const std::vector<VkSemaphore>& signal_semaphores,
      const std::vector<VkCommandBuffer>& command_buffers,
      const VulkanHandle<VkFence>& fence) const;

  [[nodiscard]] bool WaitIdle() const;

 private:
  VulkanProcTable& vk_;
  VulkanHandle<VkPhysicalDevice> physical_device_;
  VulkanHandle<VkDevice> device_;
  VulkanHandle<VkQueue> queue_;
  VulkanHandle<VkCommandPool> command_pool_;
  uint32_t graphics_queue_index_;
  bool valid_;

  bool InitializeCommandPool();
  std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties() const;

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanDevice);
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_DEVICE_H_
