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

#ifndef APPCODE_SHELL_GPU_GPU_SURFACE_VULKAN_H_
#define APPCODE_SHELL_GPU_GPU_SURFACE_VULKAN_H_

#include <memory>

#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/shell/gpu/gpu_surface_vulkan_delegate.h"
#include "appcode/vulkan/vulkan_backbuffer.h"
#include "appcode/vulkan/vulkan_native_surface.h"
#include "appcode/vulkan/vulkan_window.h"

#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief  A GPU surface backed by VkImages provided by a
///         GPUSurfaceVulkanDelegate.
///
class GPUSurfaceVulkan : public Surface {
 public:
  //------------------------------------------------------------------------------
  /// @brief      Create a GPUSurfaceVulkan while letting it reuse an existing
  ///             GrDirectContext.
  ///
  GPUSurfaceVulkan(GPUSurfaceVulkanDelegate* delegate,
                   const sk_sp<GrDirectContext>& context,
                   bool render_to_surface);

  ~GPUSurfaceVulkan() override;

  // |Surface|
  bool IsValid() override;

  // |Surface|
  std::unique_ptr<SurfaceFrame> AcquireFrame(const SkISize& size) override;

  // |Surface|
  SkMatrix GetRootTransformation() const override;

  // |Surface|
  GrDirectContext* GetContext() override;

  static SkColorType ColorTypeFromFormat(const VkFormat format);

 private:
  GPUSurfaceVulkanDelegate* delegate_;
  sk_sp<GrDirectContext> skia_context_;
  bool render_to_surface_;

  fml::WeakPtrFactory<GPUSurfaceVulkan> weak_factory_;

  sk_sp<SkSurface> CreateSurfaceFromVulkanImage(const VkImage image,
                                                const VkFormat format,
                                                const SkISize& size);

  FML_DISALLOW_COPY_AND_ASSIGN(GPUSurfaceVulkan);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_GPU_GPU_SURFACE_VULKAN_H_
