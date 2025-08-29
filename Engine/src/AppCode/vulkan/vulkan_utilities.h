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

#ifndef APPCODE_VULKAN_VULKAN_UTILITIES_H_
#define APPCODE_VULKAN_VULKAN_UTILITIES_H_

#include <string>
#include <vector>

#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_handle.h"
#include "appcode/vulkan/procs/vulkan_proc_table.h"

namespace vulkan {

bool ValidationLayerInfoMessagesEnabled();
bool ValidationErrorsFatal();

std::vector<std::string> InstanceLayersToEnable(const VulkanProcTable& vk,
                                                bool enable_validation_layers);

std::vector<std::string> DeviceLayersToEnable(
    const VulkanProcTable& vk,
    const VulkanHandle<VkPhysicalDevice>& physical_device,
    bool enable_validation_layers);

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_UTILITIES_H_
