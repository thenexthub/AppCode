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

#ifndef APPCODE_SHELL_GPU_GPU_SURFACE_GL_IMPELLER_H_
#define APPCODE_SHELL_GPU_GPU_SURFACE_GL_IMPELLER_H_

#include "appcode/common/graphics/gl_context_switch.h"
#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/impeller/display_list/aiks_context.h"
#include "appcode/impeller/renderer/context.h"
#include "appcode/shell/gpu/gpu_surface_gl_delegate.h"

namespace appcode {

class GPUSurfaceGLImpeller final : public Surface {
 public:
  explicit GPUSurfaceGLImpeller(GPUSurfaceGLDelegate* delegate,
                                std::shared_ptr<impeller::Context> context,
                                bool render_to_surface);

  // |Surface|
  ~GPUSurfaceGLImpeller() override;

  // |Surface|
  bool IsValid() override;

 private:
  GPUSurfaceGLDelegate* delegate_ = nullptr;
  std::shared_ptr<impeller::Context> impeller_context_;
  bool render_to_surface_ = true;
  std::shared_ptr<impeller::AiksContext> aiks_context_;
  bool is_valid_ = false;
  fml::TaskRunnerAffineWeakPtrFactory<GPUSurfaceGLImpeller> weak_factory_;

  // |Surface|
  std::unique_ptr<SurfaceFrame> AcquireFrame(const SkISize& size) override;

  // |Surface|
  SkMatrix GetRootTransformation() const override;

  // |Surface|
  GrDirectContext* GetContext() override;

  // |Surface|
  std::unique_ptr<GLContextResult> MakeRenderContextCurrent() override;

  // |Surface|
  bool ClearRenderContext() override;

  // |Surface|
  bool AllowsDrawingWhenGpuDisabled() const override;

  // |Surface|
  bool EnableRasterCache() const override;

  // |Surface|
  std::shared_ptr<impeller::AiksContext> GetAiksContext() const override;

  FML_DISALLOW_COPY_AND_ASSIGN(GPUSurfaceGLImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_GPU_GPU_SURFACE_GL_IMPELLER_H_
