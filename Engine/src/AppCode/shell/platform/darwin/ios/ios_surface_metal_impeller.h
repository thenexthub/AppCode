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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_SURFACE_METAL_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_SURFACE_METAL_IMPELLER_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/gpu/gpu_surface_metal_delegate.h"
#include "appcode/shell/platform/darwin/ios/ios_surface.h"

@class CAMetalLayer;

namespace impeller {
class Context;
}  // namespace impeller

namespace appcode {

class SK_API_AVAILABLE_CA_METAL_LAYER IOSSurfaceMetalImpeller final
    : public IOSSurface,
      public GPUSurfaceMetalDelegate {
 public:
  IOSSurfaceMetalImpeller(CAMetalLayer* layer, const std::shared_ptr<IOSContext>& context);

  // |IOSSurface|
  ~IOSSurfaceMetalImpeller();

 private:
  CAMetalLayer* layer_;
  const std::shared_ptr<impeller::Context> impeller_context_;
  std::shared_ptr<impeller::AiksContext> aiks_context_;
  bool is_valid_ = false;

  // |IOSSurface|
  bool IsValid() const override;

  // |IOSSurface|
  void UpdateStorageSizeIfNecessary() override;

  // |IOSSurface|
  std::unique_ptr<Surface> CreateGPUSurface() override;

  // |GPUSurfaceMetalDelegate|
  GPUCAMetalLayerHandle GetCAMetalLayer(const SkISize& frame_info) const override
      __attribute__((cf_audited_transfer));

  // |GPUSurfaceMetalDelegate|
  bool PresentDrawable(GrMTLHandle drawable) const override __attribute__((cf_audited_transfer));

  // |GPUSurfaceMetalDelegate|
  GPUMTLTextureInfo GetMTLTexture(const SkISize& frame_info) const override
      __attribute__((cf_audited_transfer));

  // |GPUSurfaceMetalDelegate|
  bool PresentTexture(GPUMTLTextureInfo texture) const override
      __attribute__((cf_audited_transfer));

  // |GPUSurfaceMetalDelegate|
  bool AllowsDrawingWhenGpuDisabled() const override;

  FML_DISALLOW_COPY_AND_ASSIGN(IOSSurfaceMetalImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_SURFACE_METAL_IMPELLER_H_
