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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_VULKAN_SWAPCHAIN_VK_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_VULKAN_SWAPCHAIN_VK_H_

#include "impeller/renderer/backend/vulkan/swapchain/swapchain_vk.h"
#include "impeller/toolkit/interop/backend/vulkan/surface_vk.h"
#include "impeller/toolkit/interop/context.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class SwapchainVK final
    : public Object<SwapchainVK,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerVulkanSwapchain)> {
 public:
  SwapchainVK(Context& context, VkSurfaceKHR surface);

  ~SwapchainVK();

  bool IsValid() const;

  SwapchainVK(const SwapchainVK&) = delete;

  SwapchainVK& operator=(const SwapchainVK&) = delete;

  ScopedObject<SurfaceVK> AcquireNextSurface();

 private:
  ScopedObject<Context> context_;
  std::shared_ptr<impeller::SwapchainVK> swapchain_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_VULKAN_SWAPCHAIN_VK_H_
