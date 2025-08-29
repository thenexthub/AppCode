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

#include "display_list/dl_blend_mode.h"
#include "display_list/dl_color.h"
#include "display_list/dl_tile_mode.h"
#include "appcode/testing/testing.h"
#include "impeller/core/formats.h"
#include "impeller/display_list/skia_conversions.h"
#include "impeller/geometry/color.h"
#include "impeller/geometry/scalar.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRRect.h"

namespace impeller {
namespace testing {

TEST(SkiaConversionTest, ToSamplerDescriptor) {
  EXPECT_EQ(skia_conversions::ToSamplerDescriptor(
                appcode::DlImageSampling::kNearestNeighbor)
                .min_filter,
            impeller::MinMagFilter::kNearest);
  EXPECT_EQ(skia_conversions::ToSamplerDescriptor(
                appcode::DlImageSampling::kNearestNeighbor)
                .mip_filter,
            impeller::MipFilter::kBase);

  EXPECT_EQ(
      skia_conversions::ToSamplerDescriptor(appcode::DlImageSampling::kLinear)
          .min_filter,
      impeller::MinMagFilter::kLinear);
  EXPECT_EQ(
      skia_conversions::ToSamplerDescriptor(appcode::DlImageSampling::kLinear)
          .mip_filter,
      impeller::MipFilter::kBase);

  EXPECT_EQ(skia_conversions::ToSamplerDescriptor(
                appcode::DlImageSampling::kMipmapLinear)
                .min_filter,
            impeller::MinMagFilter::kLinear);
  EXPECT_EQ(skia_conversions::ToSamplerDescriptor(
                appcode::DlImageSampling::kMipmapLinear)
                .mip_filter,
            impeller::MipFilter::kLinear);
}

TEST(SkiaConversionsTest, ToColor) {
  // Create a color with alpha, red, green, and blue values that are all
  // trivially divisible by 255 so that we can test the conversion results in
  // correct scalar values.
  //                                                AARRGGBB
  const appcode::DlColor color = appcode::DlColor(0x8040C020);
  auto converted_color = skia_conversions::ToColor(color);

  ASSERT_TRUE(ScalarNearlyEqual(converted_color.alpha, 0x80 * (1.0f / 255)));
  ASSERT_TRUE(ScalarNearlyEqual(converted_color.red, 0x40 * (1.0f / 255)));
  ASSERT_TRUE(ScalarNearlyEqual(converted_color.green, 0xC0 * (1.0f / 255)));
  ASSERT_TRUE(ScalarNearlyEqual(converted_color.blue, 0x20 * (1.0f / 255)));
}

}  // namespace testing
}  // namespace impeller
