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

#ifndef APPCODE_SHELL_GPU_GPU_SURFACE_VULKAN_DELEGATE_H_
#define APPCODE_SHELL_GPU_GPU_SURFACE_VULKAN_DELEGATE_H_

#include "appcode/fml/memory/ref_ptr.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/vulkan/procs/vulkan_proc_table.h"
#include "appcode/vulkan/vulkan_device.h"
#include "appcode/vulkan/vulkan_image.h"
#include "third_party/skia/include/core/SkSize.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      Interface implemented by all platform surfaces that can present
///             a Vulkan backing store to the "screen". The GPU surface
///             abstraction (which abstracts the client rendering API) uses this
///             delegation pattern to tell the platform surface (which abstracts
///             how backing stores fulfilled by the selected client rendering
///             API end up on the "screen" on a particular platform) when the
///             rasterizer needs to allocate and present the Vulkan backing
///             store.
///
/// @see        |EmbedderSurfaceVulkan|.
///
class GPUSurfaceVulkanDelegate {
 public:
  virtual ~GPUSurfaceVulkanDelegate();

  /// @brief  Obtain a reference to the Vulkan implementation's proc table.
  ///
  virtual const vulkan::VulkanProcTable& vk() = 0;

  /// @brief  Called by the engine to fetch a VkImage for writing the next
  ///         frame.
  ///
  virtual FlutterVulkanImage AcquireImage(const SkISize& size) = 0;

  /// @brief  Called by the engine once a frame has been rendered to the image
  ///         and it's ready to be bound for further reading/writing.
  ///
  virtual bool PresentImage(VkImage image, VkFormat format) = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_GPU_GPU_SURFACE_VULKAN_DELEGATE_H_
