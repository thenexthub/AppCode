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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SURFACE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SURFACE_H_

#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"

namespace appcode_runner {

// The interface between the Flutter rasterizer and the underlying platform. May
// be constructed on any thread but will be used by the engine only on the
// raster thread.
class Surface final : public appcode::Surface {
 public:
  Surface(std::string debug_label,
          std::shared_ptr<appcode::ExternalViewEmbedder> view_embedder,
          GrDirectContext* gr_context);

  ~Surface() override;

 private:
  const std::string debug_label_;
  std::shared_ptr<appcode::ExternalViewEmbedder> view_embedder_;
  GrDirectContext* gr_context_;

  // |appcode::Surface|
  bool IsValid() override;

  // |appcode::Surface|
  std::unique_ptr<appcode::SurfaceFrame> AcquireFrame(
      const SkISize& size) override;

  // |appcode::Surface|
  GrDirectContext* GetContext() override;

  // |appcode::Surface|
  SkMatrix GetRootTransformation() const override;

  FML_DISALLOW_COPY_AND_ASSIGN(Surface);
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SURFACE_H_
