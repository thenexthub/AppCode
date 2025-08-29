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

#import "flutter/shell/platform/darwin/ios/ios_context_metal_impeller.h"

#include "flutter/impeller/entity/mtl/entity_shaders.h"
#import "flutter/shell/platform/darwin/ios/ios_external_texture_metal.h"
#include "impeller/display_list/aiks_context.h"
#include "impeller/typographer/backends/skia/typographer_context_skia.h"

APPCODE_ASSERT_ARC

namespace flutter {
namespace {
impeller::Flags SettingsToFlags(const Settings& settings) {
  return impeller::Flags{
      .antialiased_lines = settings.impeller_antialiased_lines,
  };
}
}  // namespace

IOSContextMetalImpeller::IOSContextMetalImpeller(
    const Settings& settings,
    const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch)
    : darwin_context_metal_impeller_([[FlutterDarwinContextMetalImpeller alloc]
                           init:SettingsToFlags(settings)
          gpuDisabledSyncSwitch:is_gpu_disabled_sync_switch]) {
  if (darwin_context_metal_impeller_.context) {
    aiks_context_ = std::make_shared<impeller::AiksContext>(
        darwin_context_metal_impeller_.context, impeller::TypographerContextSkia::Make());
  }
}

IOSContextMetalImpeller::~IOSContextMetalImpeller() = default;

IOSRenderingBackend IOSContextMetalImpeller::GetBackend() const {
  return IOSRenderingBackend::kImpeller;
}

// |IOSContext|
std::shared_ptr<impeller::Context> IOSContextMetalImpeller::GetImpellerContext() const {
  return darwin_context_metal_impeller_.context;
}

// |IOSContext|
std::shared_ptr<impeller::AiksContext> IOSContextMetalImpeller::GetAiksContext() const {
  return aiks_context_;
}

// |IOSContext|
std::unique_ptr<Texture> IOSContextMetalImpeller::CreateExternalTexture(
    int64_t texture_id,
    NSObject<FlutterTexture>* texture) {
  return std::make_unique<IOSExternalTextureMetal>([darwin_context_metal_impeller_
      createExternalTextureWithIdentifier:texture_id
                                  texture:texture]);
}

}  // namespace flutter
