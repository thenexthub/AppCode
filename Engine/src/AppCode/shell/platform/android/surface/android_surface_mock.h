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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_ANDROID_SURFACE_MOCK_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_ANDROID_SURFACE_MOCK_H_

#include "appcode/shell/gpu/gpu_surface_gl_skia.h"
#include "appcode/shell/platform/android/surface/android_surface.h"
#include "gmock/gmock.h"

namespace appcode {

//------------------------------------------------------------------------------
/// Mock for |AndroidSurface|. This implementation can be used in unit
/// tests without requiring the Android toolchain.
///
class AndroidSurfaceMock final : public GPUSurfaceGLDelegate,
                                 public AndroidSurface {
 public:
  MOCK_METHOD(bool, IsValid, (), (const, override));

  MOCK_METHOD(void, TeardownOnScreenContext, (), (override));

  MOCK_METHOD(std::unique_ptr<Surface>,
              CreateGPUSurface,
              (GrDirectContext * gr_context),
              (override));

  MOCK_METHOD(bool, OnScreenSurfaceResize, (const SkISize& size), (override));

  MOCK_METHOD(bool, ResourceContextMakeCurrent, (), (override));

  MOCK_METHOD(bool, ResourceContextClearCurrent, (), (override));

  MOCK_METHOD(bool,
              SetNativeWindow,
              (fml::RefPtr<AndroidNativeWindow> window,
               const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade),
              (override));

  // |GPUSurfaceGLDelegate|
  std::unique_ptr<GLContextResult> GLContextMakeCurrent() override;

  // |GPUSurfaceGLDelegate|
  bool GLContextClearCurrent() override;

  // |GPUSurfaceGLDelegate|
  bool GLContextPresent(const GLPresentInfo& present_info) override;

  // |GPUSurfaceGLDelegate|
  GLFBOInfo GLContextFBO(GLFrameInfo frame_info) const override;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_ANDROID_SURFACE_MOCK_H_
