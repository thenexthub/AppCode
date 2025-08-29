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

#include "vulkan_surface.h"

#include "vulkan_application.h"
#include "vulkan_native_surface.h"

namespace vulkan {

VulkanSurface::VulkanSurface(
    VulkanProcTable& p_vk,           // NOLINT
    VulkanApplication& application,  // NOLINT
    std::unique_ptr<VulkanNativeSurface> native_surface)
    : vk(p_vk),
      application_(application),
      native_surface_(std::move(native_surface)),
      valid_(false) {
  if (native_surface_ == nullptr || !native_surface_->IsValid()) {
    FML_DLOG(INFO) << "Native surface was invalid.";
    return;
  }

  VkSurfaceKHR surface =
      native_surface_->CreateSurfaceHandle(vk, application.GetInstance());

  if (surface == VK_NULL_HANDLE) {
    FML_DLOG(INFO) << "Could not create the surface handle.";
    return;
  }

  surface_ = VulkanHandle<VkSurfaceKHR>{
      surface, [this](VkSurfaceKHR surface) {
        vk.DestroySurfaceKHR(application_.GetInstance(), surface, nullptr);
      }};

  valid_ = true;
}

VulkanSurface::~VulkanSurface() = default;

bool VulkanSurface::IsValid() const {
  return valid_;
}

const VulkanHandle<VkSurfaceKHR>& VulkanSurface::Handle() const {
  return surface_;
}

const VulkanNativeSurface& VulkanSurface::GetNativeSurface() const {
  return *native_surface_;
}

SkISize VulkanSurface::GetSize() const {
  return valid_ ? native_surface_->GetSize() : SkISize::Make(0, 0);
}

}  // namespace vulkan
