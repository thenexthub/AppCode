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

#ifndef APPCODE_SHELL_GPU_GPU_SURFACE_METAL_IMPELLER_H_
#define APPCODE_SHELL_GPU_GPU_SURFACE_METAL_IMPELLER_H_

#include <Metal/Metal.h>

#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"
#include "appcode/impeller/display_list/aiks_context.h"
#include "appcode/impeller/renderer/backend/metal/context_mtl.h"
#include "appcode/impeller/renderer/backend/metal/swapchain_transients_mtl.h"
#include "appcode/shell/gpu/gpu_surface_metal_delegate.h"
#include "third_party/skia/include/gpu/ganesh/mtl/GrMtlTypes.h"

namespace appcode {

class IMPELLER_CA_METAL_LAYER_AVAILABLE GPUSurfaceMetalImpeller
    : public Surface {
 public:
  GPUSurfaceMetalImpeller(GPUSurfaceMetalDelegate* delegate,
                          const std::shared_ptr<impeller::AiksContext>& context,
                          bool render_to_surface = true);

  // |Surface|
  ~GPUSurfaceMetalImpeller();

  // |Surface|
  bool IsValid() override;

  virtual Surface::SurfaceData GetSurfaceData() const override;

 private:
  const GPUSurfaceMetalDelegate* delegate_;
  const MTLRenderTargetType render_target_type_;
  std::shared_ptr<impeller::AiksContext> aiks_context_;
  id<MTLTexture> last_texture_;
  // TODO(38466): Refactor GPU surface APIs take into account the fact that an
  // external view embedder may want to render to the root surface. This is a
  // hack to make avoid allocating resources for the root surface when an
  // external view embedder is present.
  bool render_to_surface_ = true;
  bool disable_partial_repaint_ = false;
  // Accumulated damage for each framebuffer; Key is address of underlying
  // MTLTexture for each drawable
  std::shared_ptr<std::map<void*, SkIRect>> damage_ =
      std::make_shared<std::map<void*, SkIRect>>();
  std::shared_ptr<impeller::SwapchainTransientsMTL> swapchain_transients_;

  // |Surface|
  std::unique_ptr<SurfaceFrame> AcquireFrame(
      const SkISize& frame_size) override;

  std::unique_ptr<SurfaceFrame> AcquireFrameFromCAMetalLayer(
      const SkISize& frame_size);

  std::unique_ptr<SurfaceFrame> AcquireFrameFromMTLTexture(
      const SkISize& frame_size);

  // |Surface|
  SkMatrix GetRootTransformation() const override;

  // |Surface|
  GrDirectContext* GetContext() override;

  // |Surface|
  std::unique_ptr<GLContextResult> MakeRenderContextCurrent() override;

  // |Surface|
  bool AllowsDrawingWhenGpuDisabled() const override;

  // |Surface|
  bool EnableRasterCache() const override;

  // |Surface|
  std::shared_ptr<impeller::AiksContext> GetAiksContext() const override;

  FML_DISALLOW_COPY_AND_ASSIGN(GPUSurfaceMetalImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_GPU_GPU_SURFACE_METAL_IMPELLER_H_
