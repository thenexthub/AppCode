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

#include "appcode/flow/layers/offscreen_surface.h"

#include <memory>

#include "gtest/gtest.h"

namespace appcode::testing {

TEST(OffscreenSurfaceTest, EmptySurfaceIsInvalid) {
  auto surface = std::make_unique<OffscreenSurface>(nullptr, DlISize());
  ASSERT_FALSE(surface->IsValid());
}

TEST(OffscreenSurfaceTest, OnexOneSurfaceIsValid) {
  auto surface = std::make_unique<OffscreenSurface>(nullptr, DlISize(1, 1));
  ASSERT_TRUE(surface->IsValid());
}

TEST(OffscreenSurfaceTest, PaintSurfaceBlack) {
  auto surface = std::make_unique<OffscreenSurface>(nullptr, DlISize(1, 1));

  DlCanvas* canvas = surface->GetCanvas();
  canvas->Clear(DlColor::kBlack());
  canvas->Flush();

  auto raster_data = surface->GetRasterData(false);
  const uint32_t* actual =
      reinterpret_cast<const uint32_t*>(raster_data->data());

  // picking black as the color since byte ordering seems to matter.
  ASSERT_EQ(actual[0], 0xFF000000u);
}

}  // namespace appcode::testing
