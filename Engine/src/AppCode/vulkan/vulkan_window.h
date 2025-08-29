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

#ifndef APPCODE_VULKAN_VULKAN_WINDOW_H_
#define APPCODE_VULKAN_VULKAN_WINDOW_H_

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_proc_table.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"
#include "third_party/skia/include/gpu/vk/VulkanBackendContext.h"

namespace vulkan {

class VulkanNativeSurface;
class VulkanDevice;
class VulkanSurface;
class VulkanSwapchain;
class VulkanImage;
class VulkanApplication;
class VulkanBackbuffer;

class VulkanWindow {
 public:
  //------------------------------------------------------------------------------
  /// @brief      Construct a VulkanWindow. Let it implicitly create a
  ///             GrDirectContext.
  ///
  VulkanWindow(fml::RefPtr<VulkanProcTable> proc_table,
               std::unique_ptr<VulkanNativeSurface> native_surface);

  //------------------------------------------------------------------------------
  /// @brief      Construct a VulkanWindow. Let reuse an existing
  ///             GrDirectContext built by another VulkanWindow.
  ///
  VulkanWindow(const sk_sp<GrDirectContext>& context,
               fml::RefPtr<VulkanProcTable> proc_table,
               std::unique_ptr<VulkanNativeSurface> native_surface);

  ~VulkanWindow();

  bool IsValid() const;

  GrDirectContext* GetSkiaGrContext();

  sk_sp<SkSurface> AcquireSurface();

  bool SwapBuffers();

 private:
  bool valid_;
  fml::RefPtr<VulkanProcTable> vk_;
  std::unique_ptr<VulkanApplication> application_;
  std::unique_ptr<VulkanDevice> logical_device_;
  std::unique_ptr<VulkanSurface> surface_;
  std::unique_ptr<VulkanSwapchain> swapchain_;
  sk_sp<skgpu::VulkanMemoryAllocator> memory_allocator_;
  sk_sp<GrDirectContext> skia_gr_context_;

  bool CreateSkiaGrContext();

  bool CreateSkiaBackendContext(skgpu::VulkanBackendContext*,
                                VkPhysicalDeviceFeatures*,
                                skgpu::VulkanExtensions*);

  [[nodiscard]] bool RecreateSwapchain();

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanWindow);
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_WINDOW_H_
