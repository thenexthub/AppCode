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

#ifndef APPCODE_VULKAN_PROCS_VULKAN_INTERFACE_H_
#define APPCODE_VULKAN_PROCS_VULKAN_INTERFACE_H_

#include <string>

#include "appcode/fml/build_config.h"
#include "appcode/fml/logging.h"

#if FML_OS_ANDROID
#ifndef VK_USE_PLATFORM_ANDROID_KHR
#define VK_USE_PLATFORM_ANDROID_KHR 1
#endif  // VK_USE_PLATFORM_ANDROID_KHR
#endif  // FML_OS_ANDROID

#if OS_FUCHSIA
#ifndef VK_USE_PLATFORM_MAGMA_KHR
#define VK_USE_PLATFORM_MAGMA_KHR 1
#endif  // VK_USE_PLATFORM_MAGMA_KHR
#ifndef VK_USE_PLATFORM_FUCHSIA
#define VK_USE_PLATFORM_FUCHSIA 1
#endif  // VK_USE_PLATFORM_FUCHSIA
#endif  // OS_FUCHSIA

#include <vulkan/vulkan.h>

#define VK_CALL_LOG_ERROR(expression) VK_CALL_LOG(expression, ERROR)

#define VK_CALL_LOG_FATAL(expression) VK_CALL_LOG(expression, FATAL)

#define VK_CALL_LOG(expression, severity)                     \
  ({                                                          \
    __typeof__(expression) _rc = (expression);                \
    if (_rc != VK_SUCCESS) {                                  \
      FML_LOG(severity) << "Vulkan call '" << #expression     \
                        << "' failed with error "             \
                        << vulkan::VulkanResultToString(_rc); \
    }                                                         \
    _rc;                                                      \
  })

namespace vulkan {

std::string VulkanResultToString(VkResult result);

}  // namespace vulkan

#endif  // APPCODE_VULKAN_PROCS_VULKAN_INTERFACE_H_
