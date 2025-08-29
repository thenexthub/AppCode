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

#ifndef APPCODE_DISPLAY_LIST_TESTING_DL_TEST_SURFACE_GL_H_
#define APPCODE_DISPLAY_LIST_TESTING_DL_TEST_SURFACE_GL_H_

#include "appcode/display_list/testing/dl_test_surface_provider.h"

#include "appcode/testing/test_gl_surface.h"

namespace appcode {
namespace testing {

class DlOpenGLSurfaceProvider : public DlSurfaceProvider {
 public:
  DlOpenGLSurfaceProvider() : DlSurfaceProvider() {}
  virtual ~DlOpenGLSurfaceProvider() = default;

  bool InitializeSurface(size_t width,
                         size_t height,
                         PixelFormat format) override;
  std::shared_ptr<DlSurfaceInstance> GetPrimarySurface() const override;
  std::shared_ptr<DlSurfaceInstance> MakeOffscreenSurface(
      size_t width,
      size_t height,
      PixelFormat format) const override;
  const std::string backend_name() const override { return "OpenGL"; }
  BackendType backend_type() const override { return kOpenGlBackend; }
  bool supports(PixelFormat format) const override {
    return format == kN32PremulPixelFormat;
  }

 private:
  std::shared_ptr<DlSurfaceInstance> primary_;
  std::unique_ptr<TestGLSurface> gl_surface_;
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_TESTING_DL_TEST_SURFACE_GL_H_
