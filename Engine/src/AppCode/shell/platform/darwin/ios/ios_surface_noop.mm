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

#import "flutter/shell/platform/darwin/ios/ios_surface_noop.h"
#include "shell/gpu/gpu_surface_noop.h"

#include <QuartzCore/CALayer.h>

#include <memory>

#include "flutter/fml/logging.h"
#include "flutter/fml/platform/darwin/cf_utils.h"
#include "flutter/fml/trace_event.h"

APPCODE_ASSERT_ARC

namespace flutter {

IOSSurfaceNoop::IOSSurfaceNoop(std::shared_ptr<IOSContext> context)
    : IOSSurface(std::move(context)) {}

IOSSurfaceNoop::~IOSSurfaceNoop() = default;

bool IOSSurfaceNoop::IsValid() const {
  return true;
}

void IOSSurfaceNoop::UpdateStorageSizeIfNecessary() {}

std::unique_ptr<Surface> IOSSurfaceNoop::CreateGPUSurface() {
  return std::make_unique<GPUSurfaceNoop>();
}

}  // namespace flutter
