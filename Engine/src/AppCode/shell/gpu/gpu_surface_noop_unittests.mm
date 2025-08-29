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

#include <Foundation/Foundation.h>
#include <QuartzCore/QuartzCore.h>

#include "flutter/shell/gpu/gpu_surface_noop.h"
#include "gtest/gtest.h"
#include "impeller/entity/mtl/entity_shaders.h"
#include "impeller/entity/mtl/framebuffer_blend_shaders.h"
#include "impeller/entity/mtl/modern_shaders.h"
#include "impeller/renderer/backend/metal/context_mtl.h"

namespace flutter {
namespace testing {

TEST(GPUSurfaceNoop, InvalidImpellerContextCreatesCausesSurfaceToBeInvalid) {
  auto surface = std::make_shared<GPUSurfaceNoop>();

  EXPECT_TRUE(surface->IsValid());
}

}  // namespace testing
}  // namespace flutter
