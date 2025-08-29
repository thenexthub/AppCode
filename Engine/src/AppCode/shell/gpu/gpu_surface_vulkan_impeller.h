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

#ifndef APPCODE_SHELL_GPU_GPU_SURFACE_VULKAN_IMPELLER_H_
#define APPCODE_SHELL_GPU_GPU_SURFACE_VULKAN_IMPELLER_H_

#include "appcode/common/graphics/gl_context_switch.h"
#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/impeller/display_list/aiks_context.h"
#include "appcode/impeller/renderer/context.h"
#include "appcode/shell/gpu/gpu_surface_vulkan_delegate.h"
#include "impeller/renderer/backend/vulkan/swapchain/swapchain_transients_vk.h"

namespace appcode {

class GPUSurfaceVulkanImpeller final : public Surface {
 public:
  explicit GPUSurfaceVulkanImpeller(GPUSurfaceVulkanDelegate* delegate,
                                    std::shared_ptr<impeller::Context> context);

  // |Surface|
  ~GPUSurfaceVulkanImpeller() override;

  // |Surface|
  bool IsValid() override;

 private:
  GPUSurfaceVulkanDelegate* delegate_;
  std::shared_ptr<impeller::Context> impeller_context_;
  std::shared_ptr<impeller::AiksContext> aiks_context_;
  std::shared_ptr<impeller::SwapchainTransientsVK> transients_;
  bool is_valid_ = false;

  // |Surface|
  std::unique_ptr<SurfaceFrame> AcquireFrame(const SkISize& size) override;

  // |Surface|
  SkMatrix GetRootTransformation() const override;

  // |Surface|
  GrDirectContext* GetContext() override;

  // |Surface|
  std::unique_ptr<GLContextResult> MakeRenderContextCurrent() override;

  // |Surface|
  bool EnableRasterCache() const override;

  // |Surface|
  std::shared_ptr<impeller::AiksContext> GetAiksContext() const override;

  FML_DISALLOW_COPY_AND_ASSIGN(GPUSurfaceVulkanImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_GPU_GPU_SURFACE_VULKAN_IMPELLER_H_
