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

#include "appcode/display_list/testing/dl_test_surface_software.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace appcode {
namespace testing {

using PixelFormat = DlSurfaceProvider::PixelFormat;

bool DlSoftwareSurfaceProvider::InitializeSurface(size_t width,
                                                  size_t height,
                                                  PixelFormat format) {
  primary_ = MakeOffscreenSurface(width, height, format);
  return primary_ != nullptr;
}

std::shared_ptr<DlSurfaceInstance>
DlSoftwareSurfaceProvider::MakeOffscreenSurface(size_t width,
                                                size_t height,
                                                PixelFormat format) const {
  auto surface = SkSurfaces::Raster(MakeInfo(format, width, height));
  surface->getCanvas()->clear(SK_ColorTRANSPARENT);
  return std::make_shared<DlSurfaceInstanceBase>(surface);
}

}  // namespace testing
}  // namespace appcode
