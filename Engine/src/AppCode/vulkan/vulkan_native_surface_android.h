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

#ifndef APPCODE_VULKAN_VULKAN_NATIVE_SURFACE_ANDROID_H_
#define APPCODE_VULKAN_VULKAN_NATIVE_SURFACE_ANDROID_H_

#include "appcode/fml/macros.h"
#include "vulkan_native_surface.h"

struct ANativeWindow;
typedef struct ANativeWindow ANativeWindow;

namespace vulkan {

class VulkanNativeSurfaceAndroid : public VulkanNativeSurface {
 public:
  /// Create a native surface from the valid ANativeWindow reference. Ownership
  /// of the ANativeWindow is assumed by this instance.
  explicit VulkanNativeSurfaceAndroid(ANativeWindow* native_window);

  ~VulkanNativeSurfaceAndroid();

  const char* GetExtensionName() const override;

  VkSurfaceKHR CreateSurfaceHandle(
      VulkanProcTable& vk,
      const VulkanHandle<VkInstance>& instance) const override;

  bool IsValid() const override;

  SkISize GetSize() const override;

 private:
  ANativeWindow* native_window_;

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanNativeSurfaceAndroid);
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_NATIVE_SURFACE_ANDROID_H_
