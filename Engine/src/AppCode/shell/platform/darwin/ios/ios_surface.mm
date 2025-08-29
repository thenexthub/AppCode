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

#import "flutter/shell/platform/darwin/ios/ios_surface.h"

#include <memory>

#import "flutter/shell/platform/darwin/ios/ios_surface_metal_impeller.h"
#import "flutter/shell/platform/darwin/ios/ios_surface_noop.h"
#include "flutter/shell/platform/darwin/ios/rendering_api_selection.h"

APPCODE_ASSERT_ARC

namespace flutter {

std::unique_ptr<IOSSurface> IOSSurface::Create(std::shared_ptr<IOSContext> context,
                                               CALayer* layer) {
  FML_DCHECK(layer);
  FML_DCHECK(context);

  if (@available(iOS METAL_IOS_VERSION_BASELINE, *)) {
    if ([layer isKindOfClass:[CAMetalLayer class]]) {
      switch (context->GetBackend()) {
        case IOSRenderingBackend::kSkia:
          FML_LOG(FATAL) << "Impeller opt-out unavailable.";
          return nullptr;
        case IOSRenderingBackend::kImpeller:
          return std::make_unique<IOSSurfaceMetalImpeller>(
              static_cast<CAMetalLayer*>(layer),  // Metal layer
              std::move(context)                  // context
          );
      }
    }
  }
  return std::make_unique<IOSSurfaceNoop>(std::move(context));
}

IOSSurface::IOSSurface(std::shared_ptr<IOSContext> ios_context)
    : ios_context_(std::move(ios_context)) {
  FML_DCHECK(ios_context_);
}

IOSSurface::~IOSSurface() = default;

std::shared_ptr<IOSContext> IOSSurface::GetContext() const {
  return ios_context_;
}

}  // namespace flutter
