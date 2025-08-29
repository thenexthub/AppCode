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

#ifndef APPCODE_VULKAN_VULKAN_SURFACE_H_
#define APPCODE_VULKAN_VULKAN_SURFACE_H_

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_handle.h"
#include "third_party/skia/include/core/SkSize.h"

namespace vulkan {

class VulkanProcTable;
class VulkanApplication;
class VulkanNativeSurface;

class VulkanSurface {
 public:
  VulkanSurface(VulkanProcTable& vk,
                VulkanApplication& application,
                std::unique_ptr<VulkanNativeSurface> native_surface);

  ~VulkanSurface();

  bool IsValid() const;

  /// Returns the current size of the surface or (0, 0) if invalid.
  SkISize GetSize() const;

  const VulkanHandle<VkSurfaceKHR>& Handle() const;

  const VulkanNativeSurface& GetNativeSurface() const;

 private:
  VulkanProcTable& vk;
  VulkanApplication& application_;
  std::unique_ptr<VulkanNativeSurface> native_surface_;
  VulkanHandle<VkSurfaceKHR> surface_;
  bool valid_;

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanSurface);
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_SURFACE_H_
