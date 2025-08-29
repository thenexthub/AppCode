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

#ifndef APPCODE_SHELL_GPU_GPU_SURFACE_SOFTWARE_H_
#define APPCODE_SHELL_GPU_GPU_SURFACE_SOFTWARE_H_

#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/shell/gpu/gpu_surface_software_delegate.h"

namespace appcode {

class GPUSurfaceSoftware : public Surface {
 public:
  GPUSurfaceSoftware(GPUSurfaceSoftwareDelegate* delegate,
                     bool render_to_surface);

  ~GPUSurfaceSoftware() override;

  // |Surface|
  bool IsValid() override;

  // |Surface|
  std::unique_ptr<SurfaceFrame> AcquireFrame(const SkISize& size) override;

  // |Surface|
  SkMatrix GetRootTransformation() const override;

  // |Surface|
  GrDirectContext* GetContext() override;

 private:
  GPUSurfaceSoftwareDelegate* delegate_;
  // TODO(38466): Refactor GPU surface APIs take into account the fact that an
  // external view embedder may want to render to the root surface. This is a
  // hack to make avoid allocating resources for the root surface when an
  // external view embedder is present.
  const bool render_to_surface_;
  fml::TaskRunnerAffineWeakPtrFactory<GPUSurfaceSoftware> weak_factory_;
  FML_DISALLOW_COPY_AND_ASSIGN(GPUSurfaceSoftware);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_GPU_GPU_SURFACE_SOFTWARE_H_
