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

#ifndef APPCODE_SHELL_COMMON_SHELL_TEST_PLATFORM_VIEW_GL_H_
#define APPCODE_SHELL_COMMON_SHELL_TEST_PLATFORM_VIEW_GL_H_

#include "appcode/shell/common/shell_test_external_view_embedder.h"
#include "appcode/shell/common/shell_test_platform_view.h"
#include "appcode/shell/gpu/gpu_surface_gl_delegate.h"
#include "appcode/testing/test_gl_surface.h"
#include "impeller/renderer/backend/gles/context_gles.h"

namespace appcode {
namespace testing {

class ShellTestPlatformViewGL : public ShellTestPlatformView,
                                public GPUSurfaceGLDelegate {
 public:
  ShellTestPlatformViewGL(PlatformView::Delegate& delegate,
                          const TaskRunners& task_runners,
                          std::shared_ptr<ShellTestVsyncClock> vsync_clock,
                          CreateVsyncWaiter create_vsync_waiter,
                          std::shared_ptr<ShellTestExternalViewEmbedder>
                              shell_test_external_view_embedder);

  // |ShellTestPlatformView|
  virtual ~ShellTestPlatformViewGL() override;

  // |ShellTestPlatformView|
  virtual void SimulateVSync() override;

  // |PlatformView|
  std::shared_ptr<impeller::Context> GetImpellerContext() const {
    return impeller_context_;
  }

 private:
  std::shared_ptr<impeller::ContextGLES> impeller_context_;

  TestGLSurface gl_surface_;

  CreateVsyncWaiter create_vsync_waiter_;

  std::shared_ptr<ShellTestVsyncClock> vsync_clock_;

  std::shared_ptr<ShellTestExternalViewEmbedder>
      shell_test_external_view_embedder_;

  // |PlatformView|
  std::unique_ptr<Surface> CreateRenderingSurface() override;

  // |PlatformView|
  std::shared_ptr<ExternalViewEmbedder> CreateExternalViewEmbedder() override;

  // |PlatformView|
  std::unique_ptr<VsyncWaiter> CreateVSyncWaiter() override;

  // |PlatformView|
  PointerDataDispatcherMaker GetDispatcherMaker() override;

  // |GPUSurfaceGLDelegate|
  std::unique_ptr<GLContextResult> GLContextMakeCurrent() override;

  // |GPUSurfaceGLDelegate|
  bool GLContextClearCurrent() override;

  // |GPUSurfaceGLDelegate|
  bool GLContextPresent(const GLPresentInfo& present_info) override;

  // |GPUSurfaceGLDelegate|
  GLFBOInfo GLContextFBO(GLFrameInfo frame_info) const override;

  // |GPUSurfaceGLDelegate|
  GLProcResolver GetGLProcResolver() const override;

  FML_DISALLOW_COPY_AND_ASSIGN(ShellTestPlatformViewGL);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_SHELL_TEST_PLATFORM_VIEW_GL_H_
