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

#ifndef APPCODE_TESTING_TEST_GL_SURFACE_H_
#define APPCODE_TESTING_TEST_GL_SURFACE_H_

#include <cstdint>

#include "appcode/fml/macros.h"
#include "appcode/testing/test_gl_context.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

namespace appcode::testing {

class TestGLOnscreenOnlySurface {
 public:
  explicit TestGLOnscreenOnlySurface(SkISize surface_size);

  explicit TestGLOnscreenOnlySurface(std::shared_ptr<TestEGLContext> context,
                                     SkISize size);

  ~TestGLOnscreenOnlySurface();

  const SkISize& GetSurfaceSize() const;

  bool MakeCurrent();

  bool ClearCurrent();

  bool Present();

  uint32_t GetFramebuffer(uint32_t width, uint32_t height) const;

  void* GetProcAddress(const char* name) const;

  sk_sp<SkSurface> GetOnscreenSurface();

  sk_sp<GrDirectContext> GetGrContext();

  sk_sp<GrDirectContext> CreateGrContext();

  sk_sp<SkImage> GetRasterSurfaceSnapshot();

  uint32_t GetWindowFBOId() const;

 protected:
  using EGLSurface = void*;

  const SkISize surface_size_;
  std::shared_ptr<TestEGLContext> egl_context_;
  EGLSurface onscreen_surface_;

  sk_sp<GrDirectContext> skia_context_;

  FML_DISALLOW_COPY_AND_ASSIGN(TestGLOnscreenOnlySurface);
};

class TestGLSurface : public TestGLOnscreenOnlySurface {
 public:
  explicit TestGLSurface(SkISize surface_size);

  explicit TestGLSurface(std::shared_ptr<TestEGLContext> egl_context,
                         SkISize surface_size);

  ~TestGLSurface();

  bool MakeResourceCurrent();

 private:
  using EGLSurface = void*;

  EGLSurface offscreen_surface_;

  FML_DISALLOW_COPY_AND_ASSIGN(TestGLSurface);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_GL_SURFACE_H_
