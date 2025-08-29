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

#include "vulkan_swapchain.h"

namespace vulkan {

VulkanSwapchain::VulkanSwapchain(const VulkanProcTable& p_vk,
                                 const VulkanDevice& device,
                                 const VulkanSurface& surface,
                                 GrDirectContext* skia_context,
                                 std::unique_ptr<VulkanSwapchain> old_swapchain,
                                 uint32_t queue_family_index) {}

VulkanSwapchain::~VulkanSwapchain() = default;

bool VulkanSwapchain::IsValid() const {
  return false;
}

VulkanSwapchain::AcquireResult VulkanSwapchain::AcquireSurface() {
  return {AcquireStatus::ErrorSurfaceLost, nullptr};
}

bool VulkanSwapchain::Submit() {
  return false;
}

SkISize VulkanSwapchain::GetSize() const {
  return SkISize::Make(0, 0);
}

}  // namespace vulkan
