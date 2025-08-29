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

#ifndef APPCODE_VULKAN_VULKAN_SKIA_PROC_TABLE_H_
#define APPCODE_VULKAN_VULKAN_SKIA_PROC_TABLE_H_

#include "appcode/vulkan/procs/vulkan_proc_table.h"

#include "third_party/skia/include/gpu/vk/VulkanTypes.h"

namespace vulkan {

skgpu::VulkanGetProc CreateSkiaGetProc(
    const fml::RefPtr<vulkan::VulkanProcTable>& vk);

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_SKIA_PROC_TABLE_H_
