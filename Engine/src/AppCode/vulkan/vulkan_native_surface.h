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

#ifndef APPCODE_VULKAN_VULKAN_NATIVE_SURFACE_H_
#define APPCODE_VULKAN_VULKAN_NATIVE_SURFACE_H_

#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_handle.h"
#include "appcode/vulkan/procs/vulkan_proc_table.h"
#include "third_party/skia/include/core/SkSize.h"

namespace vulkan {

class VulkanNativeSurface {
 public:
  virtual ~VulkanNativeSurface() = default;

  virtual const char* GetExtensionName() const = 0;

  virtual VkSurfaceKHR CreateSurfaceHandle(
      VulkanProcTable& vk,
      const VulkanHandle<VkInstance>& instance) const = 0;

  virtual bool IsValid() const = 0;

  virtual SkISize GetSize() const = 0;
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_NATIVE_SURFACE_H_
