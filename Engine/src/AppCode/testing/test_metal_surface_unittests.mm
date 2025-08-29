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

#include "flutter/testing/test_metal_context.h"
#include "flutter/testing/test_metal_surface.h"
#include "flutter/testing/testing.h"

namespace flutter::testing {

#ifdef SHELL_ENABLE_METAL

TEST(TestMetalSurface, EmptySurfaceIsInvalid) {
  if (!TestMetalSurface::PlatformSupportsMetal()) {
    GTEST_SKIP();
  }

  TestMetalContext metal_context = TestMetalContext();
  auto surface = TestMetalSurface::Create(metal_context);
  ASSERT_NE(surface, nullptr);
  ASSERT_FALSE(surface->IsValid());
}

TEST(TestMetalSurface, CanCreateValidTestMetalSurface) {
  if (!TestMetalSurface::PlatformSupportsMetal()) {
    GTEST_SKIP();
  }

  TestMetalContext metal_context = TestMetalContext();
  auto surface = TestMetalSurface::Create(metal_context, SkISize::Make(100, 100));
  ASSERT_NE(surface, nullptr);
  ASSERT_TRUE(surface->IsValid());
  ASSERT_NE(surface->GetSurface(), nullptr);
  ASSERT_NE(surface->GetGrContext(), nullptr);
}

#endif

}  // namespace flutter::testing
