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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_GL_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_GL_IMPELLER_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/gpu/gpu_surface_gl_impeller.h"
#include "appcode/shell/platform/embedder/embedder_external_view_embedder.h"
#include "appcode/shell/platform/embedder/embedder_surface.h"
#include "appcode/shell/platform/embedder/embedder_surface_gl_skia.h"

namespace impeller {
class ContextGLES;
}  // namespace impeller

namespace appcode {

class ReactorWorker;

class EmbedderSurfaceGLImpeller final : public EmbedderSurface,
                                        public GPUSurfaceGLDelegate {
 public:
  EmbedderSurfaceGLImpeller(
      EmbedderSurfaceGLSkia::GLDispatchTable gl_dispatch_table,
      bool fbo_reset_after_present,
      std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder);

  ~EmbedderSurfaceGLImpeller() override;

 private:
  bool valid_ = false;
  EmbedderSurfaceGLSkia::GLDispatchTable gl_dispatch_table_;
  bool fbo_reset_after_present_;
  std::shared_ptr<impeller::ContextGLES> impeller_context_;
  std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder_;
  std::shared_ptr<ReactorWorker> worker_;

  // |EmbedderSurface|
  bool IsValid() const override;

  // |EmbedderSurface|
  std::unique_ptr<Surface> CreateGPUSurface() override;

  // |EmbedderSurface|
  std::shared_ptr<impeller::Context> CreateImpellerContext() const override;

  // |GPUSurfaceGLDelegate|
  std::unique_ptr<GLContextResult> GLContextMakeCurrent() override;

  // |GPUSurfaceGLDelegate|
  bool GLContextClearCurrent() override;

  // |GPUSurfaceGLDelegate|
  bool GLContextPresent(const GLPresentInfo& present_info) override;

  // |GPUSurfaceGLDelegate|
  GLFBOInfo GLContextFBO(GLFrameInfo frame_info) const override;

  // |GPUSurfaceGLDelegate|
  bool GLContextFBOResetAfterPresent() const override;

  // |GPUSurfaceGLDelegate|
  SkMatrix GLContextSurfaceTransformation() const override;

  // |GPUSurfaceGLDelegate|
  GLProcResolver GetGLProcResolver() const override;

  // |GPUSurfaceGLDelegate|
  SurfaceFrame::FramebufferInfo GLContextFramebufferInfo() const override;

  // |EmbedderSurface|
  sk_sp<GrDirectContext> CreateResourceContext() const override;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderSurfaceGLImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_GL_IMPELLER_H_
