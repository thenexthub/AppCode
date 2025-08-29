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

#include "impeller/toolkit/interop/backend/vulkan/swapchain_vk.h"

#include "impeller/renderer/backend/vulkan/context_vk.h"

namespace impeller::interop {

SwapchainVK::SwapchainVK(Context& context, VkSurfaceKHR c_surface)
    : context_(Ref(&context)) {
  if (!context.IsVulkan()) {
    VALIDATION_LOG << "Context is not Vulkan.";
    return;
  }

  if (!c_surface) {
    VALIDATION_LOG << "Invalid surface.";
    return;
  }

  // Creating a unique object from a raw handle requires fetching the owner
  // manually.
  auto surface = vk::UniqueSurfaceKHR(
      vk::SurfaceKHR{c_surface},
      impeller::ContextVK::Cast(*context_->GetContext()).GetInstance());
  auto swapchain = impeller::SwapchainVK::Create(context.GetContext(),  //
                                                 std::move(surface),    //
                                                 ISize::MakeWH(1, 1)    //
  );
  if (!swapchain) {
    VALIDATION_LOG << "Could not create Vulkan swapchain.";
    return;
  }
  swapchain_ = std::move(swapchain);
}

SwapchainVK::~SwapchainVK() = default;

bool SwapchainVK::IsValid() const {
  return swapchain_ && swapchain_->IsValid();
}

ScopedObject<SurfaceVK> SwapchainVK::AcquireNextSurface() {
  if (!IsValid()) {
    return nullptr;
  }

  auto impeller_surface = swapchain_->AcquireNextDrawable();
  if (!impeller_surface) {
    VALIDATION_LOG << "Could not acquire next drawable.";
    return nullptr;
  }

  auto surface = Create<SurfaceVK>(*context_, std::move(impeller_surface));
  if (!surface || !surface->IsValid()) {
    VALIDATION_LOG << "Could not create valid surface.";
    return nullptr;
  }

  return surface;
}

}  // namespace impeller::interop
