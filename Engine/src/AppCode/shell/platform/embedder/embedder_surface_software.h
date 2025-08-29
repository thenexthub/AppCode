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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_SOFTWARE_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_SOFTWARE_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/gpu/gpu_surface_software.h"
#include "appcode/shell/platform/embedder/embedder_external_view_embedder.h"
#include "appcode/shell/platform/embedder/embedder_surface.h"

#include "third_party/skia/include/core/SkSurface.h"

namespace appcode {

class EmbedderSurfaceSoftware final : public EmbedderSurface,
                                      public GPUSurfaceSoftwareDelegate {
 public:
  struct SoftwareDispatchTable {
    std::function<bool(const void* allocation, size_t row_bytes, size_t height)>
        software_present_backing_store;  // required
  };

  EmbedderSurfaceSoftware(
      SoftwareDispatchTable software_dispatch_table,
      std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder);

  ~EmbedderSurfaceSoftware() override;

 private:
  bool valid_ = false;
  SoftwareDispatchTable software_dispatch_table_;
  sk_sp<SkSurface> sk_surface_;
  std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder_;

  // |EmbedderSurface|
  bool IsValid() const override;

  // |EmbedderSurface|
  std::unique_ptr<Surface> CreateGPUSurface() override;

  // |EmbedderSurface|
  sk_sp<GrDirectContext> CreateResourceContext() const override;

  // |GPUSurfaceSoftwareDelegate|
  sk_sp<SkSurface> AcquireBackingStore(const SkISize& size) override;

  // |GPUSurfaceSoftwareDelegate|
  bool PresentBackingStore(sk_sp<SkSurface> backing_store) override;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderSurfaceSoftware);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_SOFTWARE_H_
