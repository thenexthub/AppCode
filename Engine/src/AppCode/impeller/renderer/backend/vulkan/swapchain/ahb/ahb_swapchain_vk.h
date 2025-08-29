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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_AHB_AHB_SWAPCHAIN_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_AHB_AHB_SWAPCHAIN_VK_H_

#include "impeller/renderer/backend/vulkan/swapchain/ahb/ahb_swapchain_impl_vk.h"
#include "impeller/renderer/backend/vulkan/swapchain/swapchain_vk.h"
#include "impeller/toolkit/android/native_window.h"
#include "impeller/toolkit/android/surface_control.h"
#include "impeller/toolkit/android/surface_transaction.h"

namespace impeller {

using CreateTransactionCB = std::function<android::SurfaceTransaction()>;

//------------------------------------------------------------------------------
/// @brief      The implementation of a swapchain that uses hardware buffers
///             presented to a given surface control on Android.
///
/// @warning    This swapchain implementation is not available on all Android
///             versions supported by Flutter. Perform the
///             `IsAvailableOnPlatform` check and fallback to KHR swapchains if
///             this type of swapchain cannot be created. The available building
///             blocks for these kinds of swapchains are only available on
///             Android API levels >= 29.
///
class AHBSwapchainVK final : public SwapchainVK {
 public:
  static bool IsAvailableOnPlatform();

  // |SwapchainVK|
  ~AHBSwapchainVK() override;

  AHBSwapchainVK(const AHBSwapchainVK&) = delete;

  AHBSwapchainVK& operator=(const AHBSwapchainVK&) = delete;

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

  std::weak_ptr<Context> context_;
  std::shared_ptr<android::SurfaceControl> surface_control_;
  const bool enable_msaa_;
  CreateTransactionCB cb_;
  std::shared_ptr<AHBSwapchainImplVK> impl_;

  explicit AHBSwapchainVK(const std::shared_ptr<Context>& context,
                          ANativeWindow* window,
                          const CreateTransactionCB& cb,
                          const ISize& size,
                          bool enable_msaa);
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_AHB_AHB_SWAPCHAIN_VK_H_
