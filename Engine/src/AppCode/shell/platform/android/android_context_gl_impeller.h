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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_GL_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_GL_IMPELLER_H_

#include "appcode/fml/macros.h"
#include "appcode/impeller/toolkit/egl/display.h"
#include "appcode/shell/platform/android/context/android_context.h"

namespace appcode {

class AndroidContextGLImpeller : public AndroidContext {
 public:
  AndroidContextGLImpeller(std::unique_ptr<impeller::egl::Display> display,
                           bool enable_gpu_tracing);

  ~AndroidContextGLImpeller();

  // |AndroidContext|
  bool IsValid() const override;

  // |AndroidContext|
  AndroidRenderingAPI RenderingApi() const override;

  bool ResourceContextMakeCurrent(impeller::egl::Surface* offscreen_surface);
  bool ResourceContextClearCurrent();
  std::unique_ptr<impeller::egl::Surface> CreateOffscreenSurface();
  bool OnscreenContextMakeCurrent(impeller::egl::Surface* onscreen_surface);
  bool OnscreenContextClearCurrent();
  std::unique_ptr<impeller::egl::Surface> CreateOnscreenSurface(
      EGLNativeWindowType window);

 private:
  class ReactorWorker;

  std::shared_ptr<ReactorWorker> reactor_worker_;
  std::unique_ptr<impeller::egl::Display> display_;
  std::unique_ptr<impeller::egl::Config> onscreen_config_;
  std::unique_ptr<impeller::egl::Config> offscreen_config_;
  std::unique_ptr<impeller::egl::Context> onscreen_context_;
  std::unique_ptr<impeller::egl::Context> offscreen_context_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(AndroidContextGLImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_GL_IMPELLER_H_
