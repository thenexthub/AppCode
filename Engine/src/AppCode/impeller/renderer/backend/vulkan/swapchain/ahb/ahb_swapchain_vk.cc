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

#include "impeller/renderer/backend/vulkan/swapchain/ahb/ahb_swapchain_vk.h"

#include "appcode/fml/trace_event.h"
#include "impeller/renderer/backend/vulkan/context_vk.h"
#include "impeller/renderer/backend/vulkan/formats_vk.h"
#include "impeller/renderer/backend/vulkan/swapchain/ahb/ahb_formats.h"
#include "third_party/vulkan-deps/vulkan-headers/src/include/vulkan/vulkan_enums.hpp"

namespace impeller {

bool AHBSwapchainVK::IsAvailableOnPlatform() {
  return android::SurfaceControl::IsAvailableOnPlatform() &&
         android::HardwareBuffer::IsAvailableOnPlatform();
}

AHBSwapchainVK::AHBSwapchainVK(const std::shared_ptr<Context>& context,
                               ANativeWindow* window,
                               const CreateTransactionCB& cb,
                               const ISize& size,
                               bool enable_msaa)
    : context_(context),
      surface_control_(
          std::make_shared<android::SurfaceControl>(window, "ImpellerSurface")),
      enable_msaa_(enable_msaa),
      cb_(cb) {
  UpdateSurfaceSize(size);
}

AHBSwapchainVK::~AHBSwapchainVK() = default;

// |SwapchainVK|
bool AHBSwapchainVK::IsValid() const {
  return impl_ ? impl_->IsValid() : false;
}

// |SwapchainVK|
std::unique_ptr<Surface> AHBSwapchainVK::AcquireNextDrawable() {
  if (!IsValid()) {
    return nullptr;
  }

  TRACE_EVENT0("impeller", __FUNCTION__);
  return impl_->AcquireNextDrawable();
}

// |SwapchainVK|
vk::Format AHBSwapchainVK::GetSurfaceFormat() const {
  return IsValid()
             ? ToVKImageFormat(ToPixelFormat(impl_->GetDescriptor().format))
             : vk::Format::eUndefined;
}

// |SwapchainVK|
void AHBSwapchainVK::AddFinalCommandBuffer(
    std::shared_ptr<CommandBuffer> cmd_buffer) const {
  return impl_->AddFinalCommandBuffer(cmd_buffer);
}

// |SwapchainVK|
void AHBSwapchainVK::UpdateSurfaceSize(const ISize& size) {
  if (impl_ && impl_->GetSize() == size) {
    return;
  }
  TRACE_EVENT0("impeller", __FUNCTION__);
  auto impl = AHBSwapchainImplVK::Create(context_,          //
                                         surface_control_,  //
                                         cb_,               //
                                         size,              //
                                         enable_msaa_       //
  );
  if (!impl || !impl->IsValid()) {
    VALIDATION_LOG << "Could not resize swapchain to size: " << size;
    return;
  }
  impl_ = std::move(impl);
}

}  // namespace impeller
