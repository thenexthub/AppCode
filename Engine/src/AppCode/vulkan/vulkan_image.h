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

#ifndef APPCODE_VULKAN_VULKAN_IMAGE_H_
#define APPCODE_VULKAN_VULKAN_IMAGE_H_

#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_handle.h"

namespace vulkan {

class VulkanProcTable;
class VulkanCommandBuffer;

class VulkanImage {
 public:
  explicit VulkanImage(VulkanHandle<VkImage> image);

  ~VulkanImage();

  bool IsValid() const;

  [[nodiscard]] bool InsertImageMemoryBarrier(
      const VulkanCommandBuffer& command_buffer,
      VkPipelineStageFlagBits src_pipline_bits,
      VkPipelineStageFlagBits dest_pipline_bits,
      VkAccessFlags dest_access_flags,
      VkImageLayout dest_layout);

 private:
  VulkanHandle<VkImage> handle_;
  VkImageLayout layout_;
  uint32_t /* mask of VkAccessFlagBits */ access_flags_;
  bool valid_;

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanImage);
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_IMAGE_H_
