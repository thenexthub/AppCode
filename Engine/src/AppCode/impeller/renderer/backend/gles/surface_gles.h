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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SURFACE_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SURFACE_GLES_H_

#include <functional>
#include <memory>

#include "impeller/renderer/backend/gles/gles.h"
#include "impeller/renderer/context.h"
#include "impeller/renderer/surface.h"

namespace impeller {

class SurfaceGLES final : public Surface {
 public:
  using SwapCallback = std::function<bool(void)>;

  static std::unique_ptr<Surface> WrapFBO(
      const std::shared_ptr<Context>& context,
      SwapCallback swap_callback,
      GLuint fbo,
      PixelFormat color_format,
      ISize fbo_size);

  // |Surface|
  ~SurfaceGLES() override;

 private:
  SwapCallback swap_callback_;

  SurfaceGLES(SwapCallback swap_callback, const RenderTarget& target_desc);

  // |Surface|
  bool Present() const override;

  SurfaceGLES(const SurfaceGLES&) = delete;

  SurfaceGLES& operator=(const SurfaceGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SURFACE_GLES_H_
