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

#include "appcode/display_list/testing/dl_test_surface_gl.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/ganesh/SkSurfaceGanesh.h"

namespace appcode {
namespace testing {

using PixelFormat = DlSurfaceProvider::PixelFormat;

bool DlOpenGLSurfaceProvider::InitializeSurface(size_t width,
                                                size_t height,
                                                PixelFormat format) {
  gl_surface_ = std::make_unique<TestGLSurface>(SkISize::Make(width, height));
  gl_surface_->MakeCurrent();

  primary_ = MakeOffscreenSurface(width, height, format);
  return true;
}

std::shared_ptr<DlSurfaceInstance> DlOpenGLSurfaceProvider::GetPrimarySurface()
    const {
  if (!gl_surface_->MakeCurrent()) {
    return nullptr;
  }
  return primary_;
}

std::shared_ptr<DlSurfaceInstance>
DlOpenGLSurfaceProvider::MakeOffscreenSurface(size_t width,
                                              size_t height,
                                              PixelFormat format) const {
  auto offscreen_surface = SkSurfaces::RenderTarget(
      (GrRecordingContext*)gl_surface_->GetGrContext().get(),
      skgpu::Budgeted::kNo, MakeInfo(format, width, height), 1,
      kTopLeft_GrSurfaceOrigin, nullptr, false);

  offscreen_surface->getCanvas()->clear(SK_ColorTRANSPARENT);
  return std::make_shared<DlSurfaceInstanceBase>(offscreen_surface);
}

}  // namespace testing
}  // namespace appcode
