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

#include "impeller/toolkit/interop/backend/gles/surface_gles.h"

#include "impeller/renderer/backend/gles/surface_gles.h"

namespace impeller::interop {

SurfaceGLES::SurfaceGLES(Context& context,
                         uint64_t fbo,
                         PixelFormat color_format,
                         ISize size)
    : SurfaceGLES(context,
                  impeller::SurfaceGLES::WrapFBO(
                      context.GetContext(),
                      []() { return true; },
                      fbo,
                      color_format,
                      size)) {}

SurfaceGLES::SurfaceGLES(Context& context,
                         std::shared_ptr<impeller::Surface> surface)
    : Surface(context, std::move(surface)) {}

SurfaceGLES::~SurfaceGLES() = default;

}  // namespace impeller::interop
