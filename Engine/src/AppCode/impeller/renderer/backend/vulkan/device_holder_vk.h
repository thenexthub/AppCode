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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_DEVICE_HOLDER_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_DEVICE_HOLDER_VK_H_

#include "impeller/renderer/backend/vulkan/vk.h"  // IWYU pragma: keep.

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      Holds a strong reference to the underlying logical Vulkan
///             device. This comes in handy when the context is being torn down
///             and the various components on different threads may need to
///             orchestrate safe shutdown.
///
class DeviceHolderVK {
 public:
  virtual ~DeviceHolderVK() = default;

  virtual const vk::Device& GetDevice() const = 0;

  virtual const vk::PhysicalDevice& GetPhysicalDevice() const = 0;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_DEVICE_HOLDER_VK_H_
