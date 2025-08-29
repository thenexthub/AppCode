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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_KHR_KHR_SWAPCHAIN_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_KHR_KHR_SWAPCHAIN_VK_H_

#include <memory>

#include "impeller/geometry/size.h"
#include "impeller/renderer/backend/vulkan/swapchain/swapchain_vk.h"
#include "impeller/renderer/backend/vulkan/vk.h"
#include "impeller/renderer/context.h"
#include "impeller/renderer/surface.h"

namespace impeller {

class KHRSwapchainImplVK;

//------------------------------------------------------------------------------
/// @brief      A swapchain implemented backed by VK_KHR_swapchain and
///             VK_KHR_surface.
///
class KHRSwapchainVK final : public SwapchainVK {
 public:
  ~KHRSwapchainVK();

  // |SwapchainVK|
  bool IsValid() const override;

  // |SwapchainVK|
  std::unique_ptr<Surface> AcquireNextDrawable() override;

  // |SwapchainVK|
  vk::Format GetSurfaceFormat() const override;

  // |SwapchainVK|
  void UpdateSurfaceSize(const ISize& size) override;

  // |SwapchainVK|
  void AddFinalCommandBuffer(
      std::shared_ptr<CommandBuffer> cmd_buffer) const override;

 private:
  friend class SwapchainVK;

  std::shared_ptr<KHRSwapchainImplVK> impl_;
  ISize size_;
  const bool enable_msaa_;

  KHRSwapchainVK(const std::shared_ptr<Context>& context,
                 vk::UniqueSurfaceKHR surface,
                 const ISize& size,
                 bool enable_msaa);

  KHRSwapchainVK(const KHRSwapchainVK&) = delete;

  KHRSwapchainVK& operator=(const KHRSwapchainVK&) = delete;

  std::unique_ptr<Surface> AcquireNextDrawable(size_t resize_retry_count);
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_KHR_KHR_SWAPCHAIN_VK_H_
