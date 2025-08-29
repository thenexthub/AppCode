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

#ifndef APPCODE_SHELL_GPU_GPU_SURFACE_NOOP_H_
#define APPCODE_SHELL_GPU_GPU_SURFACE_NOOP_H_

#include <Metal/Metal.h>

#include "appcode/flow/surface.h"

namespace appcode {

/// @brief A rendering surface that accepts rendering intent but does not render
///        anything.
///
/// This is useful for running on platforms that need an engine instance and
/// don't have the required drivers.
class GPUSurfaceNoop : public Surface {
 public:
  explicit GPUSurfaceNoop();

  // |Surface|
  ~GPUSurfaceNoop();

  // |Surface|
  bool IsValid() override;

  // |Surface|
  Surface::SurfaceData GetSurfaceData() const override;

 private:
  // |Surface|
  std::unique_ptr<SurfaceFrame> AcquireFrame(
      const SkISize& frame_size) override;

  std::unique_ptr<SurfaceFrame> AcquireFrameFromCAMetalLayer(
      const SkISize& frame_size);

  std::unique_ptr<SurfaceFrame> AcquireFrameFromMTLTexture(
      const SkISize& frame_size);

  // |Surface|
  SkMatrix GetRootTransformation() const override;

  // |Surface|
  GrDirectContext* GetContext() override;

  // |Surface|
  std::unique_ptr<GLContextResult> MakeRenderContextCurrent() override;

  // |Surface|
  bool AllowsDrawingWhenGpuDisabled() const override;

  // |Surface|
  bool EnableRasterCache() const override;

  // |Surface|
  std::shared_ptr<impeller::AiksContext> GetAiksContext() const override;

  GPUSurfaceNoop(const GPUSurfaceNoop&) = delete;

  GPUSurfaceNoop& operator=(const GPUSurfaceNoop&) = delete;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_GPU_GPU_SURFACE_NOOP_H_
