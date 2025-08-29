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

#include "appcode/impeller/display_list/paint.h"

#include "gtest/gtest.h"

#include "appcode/display_list/dl_color.h"
#include "appcode/display_list/dl_tile_mode.h"
#include "appcode/display_list/effects/dl_color_source.h"
#include "appcode/impeller/geometry/scalar.h"

namespace impeller {
namespace testing {

TEST(PaintTest, GradientStopConversion) {
  // Typical gradient.
  std::vector<appcode::DlColor> colors = {appcode::DlColor::kBlue(),
                                          appcode::DlColor::kRed(),
                                          appcode::DlColor::kGreen()};
  std::vector<float> stops = {0.0, 0.5, 1.0};
  const auto gradient =
      appcode::DlColorSource::MakeLinear(appcode::DlPoint(0, 0),       //
                                         appcode::DlPoint(1.0, 1.0),   //
                                         3,                            //
                                         colors.data(),                //
                                         stops.data(),                 //
                                         appcode::DlTileMode::kClamp,  //
                                         nullptr                       //
      );

  std::vector<Color> converted_colors;
  std::vector<Scalar> converted_stops;
  Paint::ConvertStops(gradient->asLinearGradient(), converted_colors,
                      converted_stops);

  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[0], 0.0f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[1], 0.5f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[2], 1.0f));
}

TEST(PaintTest, GradientMissing0) {
  std::vector<appcode::DlColor> colors = {appcode::DlColor::kBlue(),
                                          appcode::DlColor::kRed()};
  std::vector<float> stops = {0.5, 1.0};
  const auto gradient =
      appcode::DlColorSource::MakeLinear(appcode::DlPoint(0, 0),       //
                                         appcode::DlPoint(1.0, 1.0),   //
                                         2,                            //
                                         colors.data(),                //
                                         stops.data(),                 //
                                         appcode::DlTileMode::kClamp,  //
                                         nullptr                       //
      );

  std::vector<Color> converted_colors;
  std::vector<Scalar> converted_stops;
  Paint::ConvertStops(gradient->asLinearGradient(), converted_colors,
                      converted_stops);

  // First color is inserted as blue.
  ASSERT_TRUE(ScalarNearlyEqual(converted_colors[0].blue, 1.0f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[0], 0.0f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[1], 0.5f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[2], 1.0f));
}

TEST(PaintTest, GradientMissingLastValue) {
  std::vector<appcode::DlColor> colors = {appcode::DlColor::kBlue(),
                                          appcode::DlColor::kRed()};
  std::vector<float> stops = {0.0, .5};
  const auto gradient =
      appcode::DlColorSource::MakeLinear(appcode::DlPoint(0, 0),       //
                                         appcode::DlPoint(1.0, 1.0),   //
                                         2,                            //
                                         colors.data(),                //
                                         stops.data(),                 //
                                         appcode::DlTileMode::kClamp,  //
                                         nullptr                       //
      );

  std::vector<Color> converted_colors;
  std::vector<Scalar> converted_stops;
  Paint::ConvertStops(gradient->asLinearGradient(), converted_colors,
                      converted_stops);

  // Last color is inserted as red.
  ASSERT_TRUE(ScalarNearlyEqual(converted_colors[2].red, 1.0f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[0], 0.0f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[1], 0.5f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[2], 1.0f));
}

TEST(PaintTest, GradientStopGreaterThan1) {
  std::vector<appcode::DlColor> colors = {appcode::DlColor::kBlue(),
                                          appcode::DlColor::kGreen(),
                                          appcode::DlColor::kRed()};
  std::vector<float> stops = {0.0, 100, 1.0};
  const auto gradient =
      appcode::DlColorSource::MakeLinear(appcode::DlPoint(0, 0),       //
                                         appcode::DlPoint(1.0, 1.0),   //
                                         3,                            //
                                         colors.data(),                //
                                         stops.data(),                 //
                                         appcode::DlTileMode::kClamp,  //
                                         nullptr                       //
      );

  std::vector<Color> converted_colors;
  std::vector<Scalar> converted_stops;
  Paint::ConvertStops(gradient->asLinearGradient(), converted_colors,
                      converted_stops);

  // Value is clamped to 1.0
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[0], 0.0f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[1], 1.0f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[2], 1.0f));
}

TEST(PaintTest, GradientConversionNonMonotonic) {
  std::vector<appcode::DlColor> colors = {
      appcode::DlColor::kBlue(), appcode::DlColor::kGreen(),
      appcode::DlColor::kGreen(), appcode::DlColor::kRed()};
  std::vector<float> stops = {0.0, 0.5, 0.4, 1.0};
  const auto gradient =
      appcode::DlColorSource::MakeLinear(appcode::DlPoint(0, 0),       //
                                         appcode::DlPoint(1.0, 1.0),   //
                                         4,                            //
                                         colors.data(),                //
                                         stops.data(),                 //
                                         appcode::DlTileMode::kClamp,  //
                                         nullptr                       //
      );

  std::vector<Color> converted_colors;
  std::vector<Scalar> converted_stops;
  Paint::ConvertStops(gradient->asLinearGradient(), converted_colors,
                      converted_stops);

  // Value is clamped to 0.5
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[0], 0.0f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[1], 0.5f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[2], 0.5f));
  ASSERT_TRUE(ScalarNearlyEqual(converted_stops[3], 1.0f));
}

}  // namespace testing
}  // namespace impeller
