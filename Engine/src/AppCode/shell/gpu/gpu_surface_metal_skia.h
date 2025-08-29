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

#ifndef APPCODE_SHELL_GPU_GPU_SURFACE_METAL_SKIA_H_
#define APPCODE_SHELL_GPU_GPU_SURFACE_METAL_SKIA_H_

#if !SLIMPELLER

#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/gpu/gpu_surface_metal_delegate.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

namespace appcode {

class SK_API_AVAILABLE_CA_METAL_LAYER GPUSurfaceMetalSkia : public Surface {
 public:
  GPUSurfaceMetalSkia(GPUSurfaceMetalDelegate* delegate,
                      sk_sp<GrDirectContext> context,
                      bool render_to_surface = true);

  // |Surface|
  ~GPUSurfaceMetalSkia();

  // |Surface|
  bool IsValid() override;

 private:
  const GPUSurfaceMetalDelegate* delegate_;
  const MTLRenderTargetType render_target_type_;
  sk_sp<GrDirectContext> context_;
  GrDirectContext* precompiled_sksl_context_ = nullptr;
  // TODO(38466): Refactor GPU surface APIs take into account the fact that an
  // external view embedder may want to render to the root surface. This is a
  // hack to make avoid allocating resources for the root surface when an
  // external view embedder is present.
  bool render_to_surface_ = true;
  bool disable_partial_repaint_ = false;

  // Accumulated damage for each framebuffer; Key is address of underlying
  // MTLTexture for each drawable
  std::map<void*, SkIRect> damage_;

  // |Surface|
  std::unique_ptr<SurfaceFrame> AcquireFrame(const SkISize& size) override;

  // |Surface|
  SkMatrix GetRootTransformation() const override;

  // |Surface|
  GrDirectContext* GetContext() override;

  // |Surface|
  std::unique_ptr<GLContextResult> MakeRenderContextCurrent() override;

  // |Surface|
  bool AllowsDrawingWhenGpuDisabled() const override;

  std::unique_ptr<SurfaceFrame> AcquireFrameFromCAMetalLayer(
      const SkISize& frame_info);

  std::unique_ptr<SurfaceFrame> AcquireFrameFromMTLTexture(
      const SkISize& frame_info);

  void PrecompileKnownSkSLsIfNecessary();

  FML_DISALLOW_COPY_AND_ASSIGN(GPUSurfaceMetalSkia);
};

}  // namespace appcode

#endif  //  !SLIMPELLER

#endif  // APPCODE_SHELL_GPU_GPU_SURFACE_METAL_SKIA_H_
