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

#include "vulkan_native_surface_android.h"

#include <android/native_window.h>

namespace vulkan {

VulkanNativeSurfaceAndroid::VulkanNativeSurfaceAndroid(
    ANativeWindow* native_window)
    : native_window_(native_window) {
  if (native_window_ == nullptr) {
    return;
  }
  ANativeWindow_acquire(native_window_);
}

VulkanNativeSurfaceAndroid::~VulkanNativeSurfaceAndroid() {
  if (native_window_ == nullptr) {
    return;
  }
  ANativeWindow_release(native_window_);
}

const char* VulkanNativeSurfaceAndroid::GetExtensionName() const {
  // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_android_surface.html
  return VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
}

VkSurfaceKHR VulkanNativeSurfaceAndroid::CreateSurfaceHandle(
    VulkanProcTable& vk,
    const VulkanHandle<VkInstance>& instance) const {
  if (!vk.IsValid() || !instance) {
    return VK_NULL_HANDLE;
  }

  const VkAndroidSurfaceCreateInfoKHR create_info = {
      .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
      .pNext = nullptr,
      .flags = 0,
      .window = native_window_,
  };

  VkSurfaceKHR surface = VK_NULL_HANDLE;

  if (VK_CALL_LOG_ERROR(vk.CreateAndroidSurfaceKHR(
          instance, &create_info, nullptr, &surface)) != VK_SUCCESS) {
    return VK_NULL_HANDLE;
  }

  return surface;
}

bool VulkanNativeSurfaceAndroid::IsValid() const {
  return native_window_ != nullptr;
}

SkISize VulkanNativeSurfaceAndroid::GetSize() const {
  return native_window_ == nullptr
             ? SkISize::Make(0, 0)
             : SkISize::Make(ANativeWindow_getWidth(native_window_),
                             ANativeWindow_getHeight(native_window_));
}

}  // namespace vulkan
