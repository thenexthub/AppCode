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

#ifndef APPCODE_FLOW_SURFACE_H_
#define APPCODE_FLOW_SURFACE_H_

#include <memory>

#include "appcode/common/graphics/gl_context_switch.h"
#include "appcode/flow/embedded_views.h"
#include "appcode/flow/surface_frame.h"
#include "appcode/fml/macros.h"

class GrDirectContext;

namespace impeller {
class AiksContext;
}  // namespace impeller

namespace appcode {

/// Abstract Base Class that represents where we will be rendering content.
class Surface {
 public:
  /// A screenshot of the surface's raw data.
  struct SurfaceData {
    std::string pixel_format;
    sk_sp<SkData> data;
  };

  Surface();

  virtual ~Surface();

  virtual bool IsValid() = 0;

  virtual std::unique_ptr<SurfaceFrame> AcquireFrame(const SkISize& size) = 0;

  virtual SkMatrix GetRootTransformation() const = 0;

  virtual GrDirectContext* GetContext() = 0;

  virtual std::unique_ptr<GLContextResult> MakeRenderContextCurrent();

  virtual bool ClearRenderContext();

  virtual bool AllowsDrawingWhenGpuDisabled() const;

  virtual bool EnableRasterCache() const;

  virtual std::shared_ptr<impeller::AiksContext> GetAiksContext() const;

  /// Capture the `SurfaceData` currently present in the surface.
  ///
  /// Not guaranteed to work on all setups and not intended to be used in
  /// production. The data field will be null if it was unable to work.
  virtual SurfaceData GetSurfaceData() const;

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(Surface);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_SURFACE_H_
