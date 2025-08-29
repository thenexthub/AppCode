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

#include <array>
#include <cmath>
#include <memory>
#include <vector>

#include "appcode/display_list/dl_blend_mode.h"
#include "appcode/display_list/dl_builder.h"
#include "appcode/display_list/dl_color.h"
#include "appcode/display_list/dl_paint.h"
#include "appcode/display_list/dl_tile_mode.h"
#include "appcode/display_list/effects/dl_color_filter.h"
#include "appcode/display_list/effects/dl_color_source.h"
#include "appcode/display_list/effects/dl_image_filters.h"
#include "appcode/display_list/effects/dl_mask_filter.h"
#include "appcode/testing/testing.h"
#include "gtest/gtest.h"
#include "impeller/display_list/aiks_context.h"
#include "impeller/display_list/dl_dispatcher.h"
#include "impeller/display_list/dl_image_impeller.h"
#include "impeller/display_list/dl_playground.h"
#include "impeller/entity/contents/clip_contents.h"
#include "impeller/entity/contents/solid_color_contents.h"
#include "impeller/entity/contents/solid_rrect_blur_contents.h"
#include "impeller/geometry/constants.h"
#include "impeller/geometry/point.h"
#include "impeller/geometry/scalar.h"
#include "impeller/playground/widgets.h"
#include "third_party/imgui/imgui.h"

namespace impeller {
namespace testing {

appcode::DlColor toColor(const float* components) {
  return appcode::DlColor(Color::ToIColor(
      Color(components[0], components[1], components[2], components[3])));
}

using DisplayListTest = DlPlayground;
INSTANTIATE_PLAYGROUND_SUITE(DisplayListTest);

TEST_P(DisplayListTest, CanDrawRect) {
  appcode::DisplayListBuilder builder;
  builder.DrawRect(DlRect::MakeXYWH(10, 10, 100, 100),
                   appcode::DlPaint(appcode::DlColor::kBlue()));
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawTextBlob) {
  appcode::DisplayListBuilder builder;
  builder.DrawTextBlob(SkTextBlob::MakeFromString("Hello", CreateTestFont()),
                       100, 100, appcode::DlPaint(appcode::DlColor::kBlue()));
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawTextBlobWithGradient) {
  appcode::DisplayListBuilder builder;

  std::vector<appcode::DlColor> colors = {appcode::DlColor::kBlue(),
                                          appcode::DlColor::kRed()};
  const float stops[2] = {0.0, 1.0};

  auto linear = appcode::DlColorSource::MakeLinear({0.0, 0.0}, {300.0, 300.0},
                                                   2, colors.data(), stops,
                                                   appcode::DlTileMode::kClamp);
  appcode::DlPaint paint;
  paint.setColorSource(linear);

  builder.DrawTextBlob(
      SkTextBlob::MakeFromString("Hello World", CreateTestFont()), 100, 100,
      paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawTextWithSaveLayer) {
  appcode::DisplayListBuilder builder;
  builder.DrawTextBlob(SkTextBlob::MakeFromString("Hello", CreateTestFont()),
                       100, 100, appcode::DlPaint(appcode::DlColor::kRed()));

  appcode::DlPaint save_paint;
  float alpha = 0.5;
  save_paint.setAlpha(static_cast<uint8_t>(255 * alpha));
  builder.SaveLayer(std::nullopt, &save_paint);
  builder.DrawTextBlob(SkTextBlob::MakeFromString("Hello with half alpha",
                                                  CreateTestFontOfSize(100)),
                       100, 300, appcode::DlPaint(appcode::DlColor::kRed()));
  builder.Restore();
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawImage) {
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  appcode::DisplayListBuilder builder;
  builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(100, 100),
                    appcode::DlImageSampling::kNearestNeighbor, nullptr);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawCapsAndJoins) {
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  paint.setDrawStyle(appcode::DlDrawStyle::kStroke);
  paint.setStrokeWidth(30);
  paint.setColor(appcode::DlColor::kRed());

  appcode::DlPathBuilder path_builder;
  path_builder.MoveTo(DlPoint(-50, 0));
  path_builder.LineTo(DlPoint(0, -50));
  path_builder.LineTo(DlPoint(50, 0));
  appcode::DlPath path(path_builder);

  builder.Translate(100, 100);
  {
    paint.setStrokeCap(appcode::DlStrokeCap::kButt);
    paint.setStrokeJoin(appcode::DlStrokeJoin::kMiter);
    paint.setStrokeMiter(4);
    builder.DrawPath(path, paint);
  }

  {
    builder.Save();
    builder.Translate(0, 100);
    // The joint in the path is 45 degrees. A miter length of 1 convert to a
    // bevel in this case.
    paint.setStrokeMiter(1);
    builder.DrawPath(path, paint);
    builder.Restore();
  }

  builder.Translate(150, 0);
  {
    paint.setStrokeCap(appcode::DlStrokeCap::kSquare);
    paint.setStrokeJoin(appcode::DlStrokeJoin::kBevel);
    builder.DrawPath(path, paint);
  }

  builder.Translate(150, 0);
  {
    paint.setStrokeCap(appcode::DlStrokeCap::kRound);
    paint.setStrokeJoin(appcode::DlStrokeJoin::kRound);
    builder.DrawPath(path, paint);
  }

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawArc) {
  auto callback = [&]() {
    static float start_angle = 45;
    static float sweep_angle = 270;
    static float stroke_width = 10;
    static bool use_center = true;

    static int selected_cap = 0;
    const char* cap_names[] = {"Butt", "Round", "Square"};
    appcode::DlStrokeCap cap;

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Start angle", &start_angle, -360, 360);
    ImGui::SliderFloat("Sweep angle", &sweep_angle, -360, 360);
    ImGui::SliderFloat("Stroke width", &stroke_width, 0, 300);
    ImGui::Combo("Cap", &selected_cap, cap_names,
                 sizeof(cap_names) / sizeof(char*));
    ImGui::Checkbox("Use center", &use_center);
    ImGui::End();

    switch (selected_cap) {
      case 0:
        cap = appcode::DlStrokeCap::kButt;
        break;
      case 1:
        cap = appcode::DlStrokeCap::kRound;
        break;
      case 2:
        cap = appcode::DlStrokeCap::kSquare;
        break;
      default:
        cap = appcode::DlStrokeCap::kButt;
        break;
    }

    static PlaygroundPoint point_a(Point(200, 200), 20, Color::White());
    static PlaygroundPoint point_b(Point(400, 400), 20, Color::White());
    auto [p1, p2] = DrawPlaygroundLine(point_a, point_b);

    appcode::DisplayListBuilder builder;
    appcode::DlPaint paint;

    Vector2 scale = GetContentScale();
    builder.Scale(scale.x, scale.y);
    paint.setDrawStyle(appcode::DlDrawStyle::kStroke);
    paint.setStrokeCap(cap);
    paint.setStrokeJoin(appcode::DlStrokeJoin::kMiter);
    paint.setStrokeMiter(10);
    auto rect = DlRect::MakeLTRB(p1.x, p1.y, p2.x, p2.y);
    paint.setColor(appcode::DlColor::kGreen());
    paint.setStrokeWidth(2);
    builder.DrawRect(rect, paint);
    paint.setColor(appcode::DlColor::kRed());
    paint.setStrokeWidth(stroke_width);
    builder.DrawArc(rect, start_angle, sweep_angle, use_center, paint);

    return builder.Build();
  };
  ASSERT_TRUE(OpenPlaygroundHere(callback));
}

TEST_P(DisplayListTest, StrokedPathsDrawCorrectly) {
  auto callback = [&]() {
    appcode::DisplayListBuilder builder;
    appcode::DlPaint paint;

    paint.setColor(appcode::DlColor::kRed());
    paint.setDrawStyle(appcode::DlDrawStyle::kStroke);

    static float stroke_width = 10.0f;
    static int selected_stroke_type = 0;
    static int selected_join_type = 0;
    const char* stroke_types[] = {"Butte", "Round", "Square"};
    const char* join_type[] = {"kMiter", "Round", "kBevel"};

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Combo("Cap", &selected_stroke_type, stroke_types,
                 sizeof(stroke_types) / sizeof(char*));
    ImGui::Combo("Join", &selected_join_type, join_type,
                 sizeof(join_type) / sizeof(char*));
    ImGui::SliderFloat("Stroke Width", &stroke_width, 10.0f, 50.0f);
    ImGui::End();

    appcode::DlStrokeCap cap;
    appcode::DlStrokeJoin join;
    switch (selected_stroke_type) {
      case 0:
        cap = appcode::DlStrokeCap::kButt;
        break;
      case 1:
        cap = appcode::DlStrokeCap::kRound;
        break;
      case 2:
        cap = appcode::DlStrokeCap::kSquare;
        break;
      default:
        cap = appcode::DlStrokeCap::kButt;
        break;
    }
    switch (selected_join_type) {
      case 0:
        join = appcode::DlStrokeJoin::kMiter;
        break;
      case 1:
        join = appcode::DlStrokeJoin::kRound;
        break;
      case 2:
        join = appcode::DlStrokeJoin::kBevel;
        break;
      default:
        join = appcode::DlStrokeJoin::kMiter;
        break;
    }
    paint.setStrokeCap(cap);
    paint.setStrokeJoin(join);
    paint.setStrokeWidth(stroke_width);

    // Make rendering better to watch.
    builder.Scale(1.5f, 1.5f);

    // Rectangle
    builder.Translate(100, 100);
    builder.DrawRect(DlRect::MakeWH(100, 100), paint);

    // Rounded rectangle
    builder.Translate(150, 0);
    builder.DrawRoundRect(
        DlRoundRect::MakeRectXY(DlRect::MakeWH(100, 50), 10, 10), paint);

    // Double rounded rectangle
    builder.Translate(150, 0);
    builder.DrawDiffRoundRect(
        DlRoundRect::MakeRectXY(DlRect::MakeWH(100, 50), 10, 10),
        DlRoundRect::MakeRectXY(DlRect::MakeXYWH(10, 10, 80, 30), 10, 10),
        paint);

    // Contour with duplicate join points
    {
      builder.Translate(150, 0);
      appcode::DlPathBuilder path_builder;
      path_builder.MoveTo(DlPoint(0, 0));
      path_builder.LineTo(DlPoint(0, 0));
      path_builder.LineTo(DlPoint(100, 0));
      path_builder.LineTo(DlPoint(100, 0));
      path_builder.LineTo(DlPoint(100, 100));
      builder.DrawPath(DlPath(path_builder), paint);
    }

    // Contour with duplicate start and end points

    // Line.
    builder.Translate(200, 0);
    {
      builder.Save();

      appcode::DlPathBuilder line_path_builder;
      line_path_builder.MoveTo(DlPoint(0, 0));
      line_path_builder.MoveTo(DlPoint(0, 0));
      line_path_builder.LineTo(DlPoint(0, 0));
      line_path_builder.LineTo(DlPoint(0, 0));
      line_path_builder.LineTo(DlPoint(50, 50));
      line_path_builder.LineTo(DlPoint(50, 50));
      line_path_builder.LineTo(DlPoint(100, 0));
      line_path_builder.LineTo(DlPoint(100, 0));
      DlPath line_path(line_path_builder);
      builder.DrawPath(line_path, paint);

      builder.Translate(0, 100);
      builder.DrawPath(line_path, paint);

      builder.Translate(0, 100);
      appcode::DlPathBuilder line_path_builder2;
      line_path_builder2.MoveTo(DlPoint(0, 0));
      line_path_builder2.LineTo(DlPoint(0, 0));
      line_path_builder2.LineTo(DlPoint(0, 0));
      builder.DrawPath(DlPath(line_path_builder2), paint);

      builder.Restore();
    }

    // Cubic.
    builder.Translate(150, 0);
    {
      builder.Save();

      appcode::DlPathBuilder cubic_path;
      cubic_path.MoveTo(DlPoint(0, 0));
      cubic_path.CubicCurveTo(DlPoint(0, 0),          //
                              DlPoint(140.0, 100.0),  //
                              DlPoint(140, 20));
      builder.DrawPath(DlPath(cubic_path), paint);

      builder.Translate(0, 100);
      appcode::DlPathBuilder cubic_path2;
      cubic_path2.MoveTo(DlPoint(0, 0));
      cubic_path2.CubicCurveTo(DlPoint(0, 0),  //
                               DlPoint(0, 0),  //
                               DlPoint(150, 150));
      builder.DrawPath(DlPath(cubic_path2), paint);

      builder.Translate(0, 100);
      appcode::DlPathBuilder cubic_path3;
      cubic_path3.MoveTo(DlPoint(0, 0));
      cubic_path3.CubicCurveTo(DlPoint(0, 0),  //
                               DlPoint(0, 0),  //
                               DlPoint(0, 0));
      builder.DrawPath(DlPath(cubic_path3), paint);

      builder.Restore();
    }

    // Quad.
    builder.Translate(200, 0);
    {
      builder.Save();

      appcode::DlPathBuilder quad_path;
      quad_path.MoveTo(DlPoint(0, 0));
      quad_path.MoveTo(DlPoint(0, 0));
      quad_path.QuadraticCurveTo(DlPoint(100, 40), DlPoint(50, 80));
      builder.DrawPath(DlPath(quad_path), paint);

      builder.Translate(0, 150);
      appcode::DlPathBuilder quad_path2;
      quad_path2.MoveTo(DlPoint(0, 0));
      quad_path2.MoveTo(DlPoint(0, 0));
      quad_path2.QuadraticCurveTo(DlPoint(0, 0), DlPoint(100, 100));
      builder.DrawPath(DlPath(quad_path2), paint);

      builder.Translate(0, 100);
      appcode::DlPathBuilder quad_path3;
      quad_path3.MoveTo(DlPoint(0, 0));
      quad_path3.QuadraticCurveTo(DlPoint(0, 0), DlPoint(0, 0));
      builder.DrawPath(DlPath(quad_path3), paint);

      builder.Restore();
    }
    return builder.Build();
  };
  ASSERT_TRUE(OpenPlaygroundHere(callback));
}

TEST_P(DisplayListTest, CanDrawWithOddPathWinding) {
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  paint.setColor(appcode::DlColor::kRed());
  paint.setDrawStyle(appcode::DlDrawStyle::kFill);

  builder.Translate(300, 300);
  appcode::DlPathBuilder path_builder;
  path_builder.AddCircle(DlPoint(0, 0), 100);
  path_builder.AddCircle(DlPoint(0, 0), 50);
  builder.DrawPath(DlPath(path_builder, appcode::DlPathFillType::kOdd), paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

// Regression test for https://github.com/appcode/appcode/issues/134816.
//
// It should be possible to draw 3 lines, and not have an implicit close path.
TEST_P(DisplayListTest, CanDrawAnOpenPath) {
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  paint.setColor(appcode::DlColor::kRed());
  paint.setDrawStyle(appcode::DlDrawStyle::kStroke);
  paint.setStrokeWidth(10);

  builder.Translate(300, 300);

  // Move to (50, 50) and draw lines from:
  // 1. (50, height)
  // 2. (width, height)
  // 3. (width, 50)
  appcode::DlPathBuilder path_builder;
  path_builder.MoveTo(DlPoint(50, 50));
  path_builder.LineTo(DlPoint(50, 100));
  path_builder.LineTo(DlPoint(100, 100));
  path_builder.LineTo(DlPoint(100, 50));
  builder.DrawPath(DlPath(path_builder), paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawWithMaskBlur) {
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  // Mask blurred image.
  {
    auto filter =
        appcode::DlBlurMaskFilter(appcode::DlBlurStyle::kNormal, 10.0f);
    paint.setMaskFilter(&filter);
    builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(100, 100),
                      appcode::DlImageSampling::kNearestNeighbor, &paint);
  }

  // Mask blurred filled path.
  {
    paint.setColor(appcode::DlColor::kYellow());
    auto filter =
        appcode::DlBlurMaskFilter(appcode::DlBlurStyle::kOuter, 10.0f);
    paint.setMaskFilter(&filter);
    builder.DrawArc(DlRect::MakeXYWH(410, 110, 100, 100), 45, 270, true, paint);
  }

  // Mask blurred text.
  {
    auto filter =
        appcode::DlBlurMaskFilter(appcode::DlBlurStyle::kSolid, 10.0f);
    paint.setMaskFilter(&filter);
    builder.DrawTextBlob(
        SkTextBlob::MakeFromString("Testing", CreateTestFont()), 220, 170,
        paint);
  }

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawStrokedText) {
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  paint.setDrawStyle(appcode::DlDrawStyle::kStroke);
  paint.setColor(appcode::DlColor::kRed());
  builder.DrawTextBlob(
      SkTextBlob::MakeFromString("stoked about stroked text", CreateTestFont()),
      250, 250, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

// Regression test for https://github.com/appcode/appcode/issues/133157.
TEST_P(DisplayListTest, StrokedTextNotOffsetFromNormalText) {
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;
  auto const& text_blob = SkTextBlob::MakeFromString("00000", CreateTestFont());

  // https://api.appcode.dev/appcode/material/Colors/blue-constant.html.
  auto const& mat_blue = appcode::DlColor(0xFF2196f3);

  // Draw a blue filled rectangle so the text is easier to see.
  paint.setDrawStyle(appcode::DlDrawStyle::kFill);
  paint.setColor(mat_blue);
  builder.DrawRect(DlRect::MakeXYWH(0, 0, 500, 500), paint);

  // Draw stacked text, with stroked text on top.
  paint.setDrawStyle(appcode::DlDrawStyle::kFill);
  paint.setColor(appcode::DlColor::kWhite());
  builder.DrawTextBlob(text_blob, 250, 250, paint);

  paint.setDrawStyle(appcode::DlDrawStyle::kStroke);
  paint.setColor(appcode::DlColor::kBlack());
  builder.DrawTextBlob(text_blob, 250, 250, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, IgnoreMaskFilterWhenSavingLayer) {
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  appcode::DisplayListBuilder builder;
  auto filter = appcode::DlBlurMaskFilter(appcode::DlBlurStyle::kNormal, 10.0f);
  appcode::DlPaint paint;
  paint.setMaskFilter(&filter);
  builder.SaveLayer(std::nullopt, &paint);
  builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(100, 100),
                    appcode::DlImageSampling::kNearestNeighbor);
  builder.Restore();
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawWithBlendColorFilter) {
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  // Pipeline blended image.
  {
    auto filter = appcode::DlColorFilter::MakeBlend(
        appcode::DlColor::kYellow(), appcode::DlBlendMode::kModulate);
    paint.setColorFilter(filter);
    builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(100, 100),
                      appcode::DlImageSampling::kNearestNeighbor, &paint);
  }

  // Advanced blended image.
  {
    auto filter = appcode::DlColorFilter::MakeBlend(
        appcode::DlColor::kRed(), appcode::DlBlendMode::kScreen);
    paint.setColorFilter(filter);
    builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(250, 250),
                      appcode::DlImageSampling::kNearestNeighbor, &paint);
  }

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawWithColorFilterImageFilter) {
  const float invert_color_matrix[20] = {
      -1, 0,  0,  0, 1,  //
      0,  -1, 0,  0, 1,  //
      0,  0,  -1, 0, 1,  //
      0,  0,  0,  1, 0,  //
  };
  auto texture = CreateTextureForFixture("boston.jpg");
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  auto color_filter = appcode::DlColorFilter::MakeMatrix(invert_color_matrix);
  auto image_filter = appcode::DlImageFilter::MakeColorFilter(color_filter);

  paint.setImageFilter(image_filter);
  builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(100, 100),
                    appcode::DlImageSampling::kNearestNeighbor, &paint);

  builder.Translate(0, 700);
  paint.setColorFilter(color_filter);
  builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(100, 100),
                    appcode::DlImageSampling::kNearestNeighbor, &paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawWithImageBlurFilter) {
  auto texture = CreateTextureForFixture("embarcadero.jpg");

  auto callback = [&]() {
    static float sigma[] = {10, 10};

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat2("Sigma", sigma, 0, 100);
    ImGui::End();

    appcode::DisplayListBuilder builder;
    appcode::DlPaint paint;

    auto filter = appcode::DlBlurImageFilter(sigma[0], sigma[1],
                                             appcode::DlTileMode::kClamp);
    paint.setImageFilter(&filter);
    builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(200, 200),
                      appcode::DlImageSampling::kNearestNeighbor, &paint);

    return builder.Build();
  };

  ASSERT_TRUE(OpenPlaygroundHere(callback));
}

TEST_P(DisplayListTest, CanDrawWithComposeImageFilter) {
  auto texture = CreateTextureForFixture("boston.jpg");
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  auto dilate = std::make_shared<appcode::DlDilateImageFilter>(10.0, 10.0);
  auto erode = std::make_shared<appcode::DlErodeImageFilter>(10.0, 10.0);
  auto open = std::make_shared<appcode::DlComposeImageFilter>(dilate, erode);
  auto close = std::make_shared<appcode::DlComposeImageFilter>(erode, dilate);

  paint.setImageFilter(open.get());
  builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(100, 100),
                    appcode::DlImageSampling::kNearestNeighbor, &paint);
  builder.Translate(0, 700);
  paint.setImageFilter(close.get());
  builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(100, 100),
                    appcode::DlImageSampling::kNearestNeighbor, &paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanClampTheResultingColorOfColorMatrixFilter) {
  auto texture = CreateTextureForFixture("boston.jpg");
  const float inner_color_matrix[20] = {
      1, 0, 0, 0, 0,  //
      0, 1, 0, 0, 0,  //
      0, 0, 1, 0, 0,  //
      0, 0, 0, 2, 0,  //
  };
  const float outer_color_matrix[20] = {
      1, 0, 0, 0,   0,  //
      0, 1, 0, 0,   0,  //
      0, 0, 1, 0,   0,  //
      0, 0, 0, 0.5, 0,  //
  };
  auto inner_color_filter =
      appcode::DlColorFilter::MakeMatrix(inner_color_matrix);
  auto outer_color_filter =
      appcode::DlColorFilter::MakeMatrix(outer_color_matrix);
  auto inner = appcode::DlImageFilter::MakeColorFilter(inner_color_filter);
  auto outer = appcode::DlImageFilter::MakeColorFilter(outer_color_filter);
  auto compose = std::make_shared<appcode::DlComposeImageFilter>(outer, inner);

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;
  paint.setImageFilter(compose.get());
  builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(100, 100),
                    appcode::DlImageSampling::kNearestNeighbor, &paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawBackdropFilter) {
  auto texture = CreateTextureForFixture("embarcadero.jpg");

  auto callback = [&]() {
    static float sigma[] = {10, 10};
    static float ctm_scale = 1;
    static bool use_bounds = true;
    static bool draw_circle = true;
    static bool add_clip = true;

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat2("Sigma", sigma, 0, 100);
    ImGui::SliderFloat("Scale", &ctm_scale, 0, 10);
    ImGui::NewLine();
    ImGui::TextWrapped(
        "If everything is working correctly, none of the options below should "
        "impact the filter's appearance.");
    ImGui::Checkbox("Use SaveLayer bounds", &use_bounds);
    ImGui::Checkbox("Draw child element", &draw_circle);
    ImGui::Checkbox("Add pre-clip", &add_clip);
    ImGui::End();

    appcode::DisplayListBuilder builder;

    Vector2 scale = ctm_scale * GetContentScale();
    builder.Scale(scale.x, scale.y);

    auto filter = appcode::DlBlurImageFilter(sigma[0], sigma[1],
                                             appcode::DlTileMode::kClamp);

    std::optional<DlRect> bounds;
    if (use_bounds) {
      static PlaygroundPoint point_a(Point(350, 150), 20, Color::White());
      static PlaygroundPoint point_b(Point(800, 600), 20, Color::White());
      auto [p1, p2] = DrawPlaygroundLine(point_a, point_b);
      bounds = DlRect::MakeLTRB(p1.x, p1.y, p2.x, p2.y);
    }

    // Insert a clip to test that the backdrop filter handles stencil depths > 0
    // correctly.
    if (add_clip) {
      builder.ClipRect(DlRect::MakeLTRB(0, 0, 99999, 99999),
                       appcode::DlClipOp::kIntersect, true);
    }

    builder.DrawImage(DlImageImpeller::Make(texture), DlPoint(200, 200),
                      appcode::DlImageSampling::kNearestNeighbor, nullptr);
    builder.SaveLayer(bounds, nullptr, &filter);

    if (draw_circle) {
      static PlaygroundPoint center_point(Point(500, 400), 20, Color::Red());
      auto circle_center = DrawPlaygroundPoint(center_point);

      appcode::DlPaint paint;
      paint.setDrawStyle(appcode::DlDrawStyle::kStroke);
      paint.setStrokeCap(appcode::DlStrokeCap::kButt);
      paint.setStrokeJoin(appcode::DlStrokeJoin::kBevel);
      paint.setStrokeWidth(10);
      paint.setColor(appcode::DlColor::kRed().withAlpha(100));
      builder.DrawCircle(DlPoint(circle_center.x, circle_center.y), 100, paint);
    }

    return builder.Build();
  };

  ASSERT_TRUE(OpenPlaygroundHere(callback));
}

TEST_P(DisplayListTest, CanDrawNinePatchImage) {
  // Image is drawn with corners to scale and center pieces stretched to fit.
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  appcode::DisplayListBuilder builder;
  auto size = texture->GetSize();
  builder.DrawImageNine(
      DlImageImpeller::Make(texture),
      DlIRect::MakeLTRB(size.width / 4, size.height / 4, size.width * 3 / 4,
                        size.height * 3 / 4),
      DlRect::MakeLTRB(0, 0, size.width * 2, size.height * 2),
      appcode::DlFilterMode::kNearest, nullptr);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawNinePatchImageCenterWidthBiggerThanDest) {
  // Edge case, the width of the corners does not leave any room for the
  // center slice. The center (across the vertical axis) is folded out of the
  // resulting image.
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  appcode::DisplayListBuilder builder;
  auto size = texture->GetSize();
  builder.DrawImageNine(
      DlImageImpeller::Make(texture),
      DlIRect::MakeLTRB(size.width / 4, size.height / 4, size.width * 3 / 4,
                        size.height * 3 / 4),
      DlRect::MakeLTRB(0, 0, size.width / 2, size.height),
      appcode::DlFilterMode::kNearest, nullptr);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawNinePatchImageCenterHeightBiggerThanDest) {
  // Edge case, the height of the corners does not leave any room for the
  // center slice. The center (across the horizontal axis) is folded out of the
  // resulting image.
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  appcode::DisplayListBuilder builder;
  auto size = texture->GetSize();
  builder.DrawImageNine(
      DlImageImpeller::Make(texture),
      DlIRect::MakeLTRB(size.width / 4, size.height / 4, size.width * 3 / 4,
                        size.height * 3 / 4),
      DlRect::MakeLTRB(0, 0, size.width, size.height / 2),
      appcode::DlFilterMode::kNearest, nullptr);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawNinePatchImageCenterBiggerThanDest) {
  // Edge case, the width and height of the corners does not leave any
  // room for the center slices. Only the corners are displayed.
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  appcode::DisplayListBuilder builder;
  auto size = texture->GetSize();
  builder.DrawImageNine(
      DlImageImpeller::Make(texture),
      DlIRect::MakeLTRB(size.width / 4, size.height / 4, size.width * 3 / 4,
                        size.height * 3 / 4),
      DlRect::MakeLTRB(0, 0, size.width / 2, size.height / 2),
      appcode::DlFilterMode::kNearest, nullptr);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawNinePatchImageCornersScaledDown) {
  // Edge case, there is not enough room for the corners to be drawn
  // without scaling them down.
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  appcode::DisplayListBuilder builder;
  auto size = texture->GetSize();
  builder.DrawImageNine(
      DlImageImpeller::Make(texture),
      DlIRect::MakeLTRB(size.width / 4, size.height / 4, size.width * 3 / 4,
                        size.height * 3 / 4),
      DlRect::MakeLTRB(0, 0, size.width / 4, size.height / 4),
      appcode::DlFilterMode::kNearest, nullptr);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, NinePatchImagePrecision) {
  // Draw a nine patch image with colored corners and verify that the corner
  // color does not leak outside the intended region.
  auto texture = CreateTextureForFixture("nine_patch_corners.png");
  appcode::DisplayListBuilder builder;
  builder.DrawImageNine(DlImageImpeller::Make(texture),
                        DlIRect::MakeXYWH(10, 10, 1, 1),
                        DlRect::MakeXYWH(0, 0, 200, 100),
                        appcode::DlFilterMode::kNearest, nullptr);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawPoints) {
  appcode::DisplayListBuilder builder;
  DlPoint points[7] = {
      {0, 0},      //
      {100, 100},  //
      {100, 0},    //
      {0, 100},    //
      {0, 0},      //
      {48, 48},    //
      {52, 52},    //
  };
  std::vector<appcode::DlStrokeCap> caps = {
      appcode::DlStrokeCap::kButt,
      appcode::DlStrokeCap::kRound,
      appcode::DlStrokeCap::kSquare,
  };
  appcode::DlPaint paint =
      appcode::DlPaint()                                         //
          .setColor(appcode::DlColor::kYellow().withAlpha(127))  //
          .setStrokeWidth(20);
  builder.Translate(50, 50);
  for (auto cap : caps) {
    paint.setStrokeCap(cap);
    builder.Save();
    builder.DrawPoints(appcode::DlPointMode::kPoints, 7, points, paint);
    builder.Translate(150, 0);
    builder.DrawPoints(appcode::DlPointMode::kLines, 5, points, paint);
    builder.Translate(150, 0);
    builder.DrawPoints(appcode::DlPointMode::kPolygon, 5, points, paint);
    builder.Restore();
    builder.Translate(0, 150);
  }
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawZeroLengthLine) {
  appcode::DisplayListBuilder builder;
  std::vector<appcode::DlStrokeCap> caps = {
      appcode::DlStrokeCap::kButt,
      appcode::DlStrokeCap::kRound,
      appcode::DlStrokeCap::kSquare,
  };
  appcode::DlPaint paint =
      appcode::DlPaint()                                         //
          .setColor(appcode::DlColor::kYellow().withAlpha(127))  //
          .setDrawStyle(appcode::DlDrawStyle::kStroke)           //
          .setStrokeCap(appcode::DlStrokeCap::kButt)             //
          .setStrokeWidth(20);
  DlPath path = DlPath::MakeLine({150, 50}, {150, 50});
  for (auto cap : caps) {
    paint.setStrokeCap(cap);
    builder.DrawLine(DlPoint(50, 50), DlPoint(50, 50), paint);
    builder.DrawPath(path, paint);
    builder.Translate(0, 150);
  }
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawShadow) {
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  auto content_scale = GetContentScale() * 0.8;
  builder.Scale(content_scale.x, content_scale.y);

  constexpr size_t star_spikes = 5;
  constexpr DlScalar half_spike_rotation = kPi / star_spikes;
  constexpr DlScalar radius = 40;
  constexpr DlScalar spike_size = 10;
  constexpr DlScalar outer_radius = radius + spike_size;
  constexpr DlScalar inner_radius = radius - spike_size;
  std::array<DlPoint, star_spikes * 2> star;
  for (size_t i = 0; i < star_spikes; i++) {
    const DlScalar rotation = half_spike_rotation * i * 2;
    star[i * 2] = DlPoint(50 + std::sin(rotation) * outer_radius,
                          50 - std::cos(rotation) * outer_radius);
    star[i * 2 + 1] =
        DlPoint(50 + std::sin(rotation + half_spike_rotation) * inner_radius,
                50 - std::cos(rotation + half_spike_rotation) * inner_radius);
  }

  std::array<DlPath, 4> paths = {
      DlPath::MakeRect(DlRect::MakeXYWH(0, 0, 200, 100)),
      DlPath::MakeRoundRectXY(DlRect::MakeXYWH(20, 0, 200, 100), 30, 30),
      DlPath::MakeCircle(DlPoint(100, 50), 50),
      DlPath::MakePoly(star.data(), star.size(), true),
  };
  paint.setColor(appcode::DlColor::kWhite());
  builder.DrawPaint(paint);
  paint.setColor(appcode::DlColor::kCyan());
  builder.Translate(100, 50);
  for (size_t x = 0; x < paths.size(); x++) {
    builder.Save();
    for (size_t y = 0; y < 6; y++) {
      builder.DrawShadow(paths[x], appcode::DlColor::kBlack(), 3 + y * 8, false,
                         1);
      builder.DrawPath(paths[x], paint);
      builder.Translate(0, 150);
    }
    builder.Restore();
    builder.Translate(250, 0);
  }

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawZeroWidthLine) {
  appcode::DisplayListBuilder builder;
  std::vector<appcode::DlStrokeCap> caps = {
      appcode::DlStrokeCap::kButt,
      appcode::DlStrokeCap::kRound,
      appcode::DlStrokeCap::kSquare,
  };
  appcode::DlPaint paint =                              //
      appcode::DlPaint()                                //
          .setColor(appcode::DlColor::kWhite())         //
          .setDrawStyle(appcode::DlDrawStyle::kStroke)  //
          .setStrokeWidth(0);
  appcode::DlPaint outline_paint =                      //
      appcode::DlPaint()                                //
          .setColor(appcode::DlColor::kYellow())        //
          .setDrawStyle(appcode::DlDrawStyle::kStroke)  //
          .setStrokeCap(appcode::DlStrokeCap::kSquare)  //
          .setStrokeWidth(1);
  DlPath path = DlPath::MakeLine({150, 50}, {160, 50});
  for (auto cap : caps) {
    paint.setStrokeCap(cap);
    builder.DrawLine(DlPoint(50, 50), DlPoint(60, 50), paint);
    builder.DrawRect(DlRect::MakeLTRB(45, 45, 65, 55), outline_paint);
    builder.DrawLine(DlPoint{100, 50}, DlPoint{100, 50}, paint);
    if (cap != appcode::DlStrokeCap::kButt) {
      builder.DrawRect(DlRect::MakeLTRB(95, 45, 105, 55), outline_paint);
    }
    builder.DrawPath(path, paint);
    builder.DrawRect(path.GetBounds().Expand(5, 5), outline_paint);
    builder.Translate(0, 150);
  }
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawWithMatrixFilter) {
  auto boston = CreateTextureForFixture("boston.jpg");

  auto callback = [&]() {
    static int selected_matrix_type = 0;
    const char* matrix_type_names[] = {"Matrix", "Local Matrix"};

    static float ctm_translation[2] = {200, 200};
    static float ctm_scale[2] = {0.65, 0.65};
    static float ctm_skew[2] = {0, 0};

    static bool enable = true;
    static float translation[2] = {100, 100};
    static float scale[2] = {0.8, 0.8};
    static float skew[2] = {0.2, 0.2};

    static bool enable_savelayer = true;

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
      ImGui::Combo("Filter type", &selected_matrix_type, matrix_type_names,
                   sizeof(matrix_type_names) / sizeof(char*));

      ImGui::TextWrapped("Current Transform");
      ImGui::SliderFloat2("CTM Translation", ctm_translation, 0, 1000);
      ImGui::SliderFloat2("CTM Scale", ctm_scale, 0, 3);
      ImGui::SliderFloat2("CTM Skew", ctm_skew, -3, 3);

      ImGui::TextWrapped(
          "MatrixFilter and LocalMatrixFilter modify the CTM in the same way. "
          "The only difference is that MatrixFilter doesn't affect the effect "
          "transform, whereas LocalMatrixFilter does.");
      // Note: See this behavior in:
      //       https://fiddle.skia.org/c/6cbb551ab36d06f163db8693972be954
      ImGui::Checkbox("Enable", &enable);
      ImGui::SliderFloat2("Filter Translation", translation, 0, 1000);
      ImGui::SliderFloat2("Filter Scale", scale, 0, 3);
      ImGui::SliderFloat2("Filter Skew", skew, -3, 3);

      ImGui::TextWrapped(
          "Rendering the filtered image within a layer can expose bounds "
          "issues. If the rendered image gets cut off when this setting is "
          "enabled, there's a coverage bug in the filter.");
      ImGui::Checkbox("Render in layer", &enable_savelayer);
    }
    ImGui::End();

    appcode::DisplayListBuilder builder;
    appcode::DlPaint paint;

    if (enable_savelayer) {
      builder.SaveLayer(std::nullopt, nullptr);
    }
    {
      auto content_scale = GetContentScale();
      builder.Scale(content_scale.x, content_scale.y);

      // Set the current transform
      auto ctm_matrix = Matrix::MakeRow(
          ctm_scale[0], ctm_skew[0], 0.0f, ctm_translation[0],  //
          ctm_skew[1], ctm_scale[1], 0.0f, ctm_translation[1],  //
          0, 0, 1, 0,                                           //
          0, 0, 0, 1);
      builder.Transform(ctm_matrix);

      // Set the matrix filter
      auto filter_matrix =
          Matrix::MakeRow(scale[0], skew[0], 0.0f, translation[0],  //
                          skew[1], scale[1], 0.0f, translation[1],  //
                          0.0f, 0.0f, 1.0f, 0.0f,                   //
                          0.0f, 0.0f, 0.0f, 1.0f);

      if (enable) {
        switch (selected_matrix_type) {
          case 0: {
            auto filter = appcode::DlMatrixImageFilter(
                filter_matrix, appcode::DlImageSampling::kLinear);
            paint.setImageFilter(&filter);
            break;
          }
          case 1: {
            auto internal_filter =
                appcode::DlBlurImageFilter(10, 10, appcode::DlTileMode::kDecal)
                    .shared();
            auto filter = appcode::DlLocalMatrixImageFilter(filter_matrix,
                                                            internal_filter);
            paint.setImageFilter(&filter);
            break;
          }
        }
      }

      builder.DrawImage(DlImageImpeller::Make(boston), DlPoint(),
                        appcode::DlImageSampling::kLinear, &paint);
    }
    if (enable_savelayer) {
      builder.Restore();
    }

    return builder.Build();
  };

  ASSERT_TRUE(OpenPlaygroundHere(callback));
}

TEST_P(DisplayListTest, CanDrawWithMatrixFilterWhenSavingLayer) {
  auto callback = [&]() {
    static float translation[2] = {0, 0};
    static bool enable_save_layer = true;

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat2("Translation", translation, -130, 130);
    ImGui::Checkbox("Enable save layer", &enable_save_layer);
    ImGui::End();

    appcode::DisplayListBuilder builder;
    builder.Save();
    builder.Scale(2.0, 2.0);
    appcode::DlPaint paint;
    paint.setColor(appcode::DlColor::kYellow());
    builder.DrawRect(DlRect::MakeWH(300, 300), paint);
    paint.setStrokeWidth(1.0);
    paint.setDrawStyle(appcode::DlDrawStyle::kStroke);
    paint.setColor(appcode::DlColor::kBlack().withAlpha(0x80));
    builder.DrawLine(DlPoint(150, 0), DlPoint(150, 300), paint);
    builder.DrawLine(DlPoint(0, 150), DlPoint(300, 150), paint);

    appcode::DlPaint save_paint;
    DlRect bounds = DlRect::MakeXYWH(100, 100, 100, 100);
    Matrix translate_matrix =
        Matrix::MakeTranslation({translation[0], translation[1]});
    if (enable_save_layer) {
      auto filter = appcode::DlMatrixImageFilter(
          translate_matrix, appcode::DlImageSampling::kNearestNeighbor);
      save_paint.setImageFilter(filter.shared());
      builder.SaveLayer(bounds, &save_paint);
    } else {
      builder.Save();
      builder.Transform(translate_matrix);
    }

    Matrix filter_matrix;
    filter_matrix.Translate({150, 150});
    filter_matrix.Scale({0.2f, 0.2f});
    filter_matrix.Translate({-150, -150});
    auto filter = appcode::DlMatrixImageFilter(
        filter_matrix, appcode::DlImageSampling::kNearestNeighbor);

    save_paint.setImageFilter(filter.shared());

    builder.SaveLayer(bounds, &save_paint);
    appcode::DlPaint paint2;
    paint2.setColor(appcode::DlColor::kBlue());
    builder.DrawRect(bounds, paint2);
    builder.Restore();
    builder.Restore();
    return builder.Build();
  };

  ASSERT_TRUE(OpenPlaygroundHere(callback));
}

TEST_P(DisplayListTest, CanDrawRectWithLinearToSrgbColorFilter) {
  appcode::DlPaint paint;
  paint.setColor(appcode::DlColor(0xFF2196F3).withAlpha(128));
  appcode::DisplayListBuilder builder;
  paint.setColorFilter(appcode::DlColorFilter::MakeLinearToSrgbGamma());
  builder.DrawRect(DlRect::MakeXYWH(0, 0, 200, 200), paint);
  builder.Translate(0, 200);

  paint.setColorFilter(appcode::DlColorFilter::MakeSrgbToLinearGamma());
  builder.DrawRect(DlRect::MakeXYWH(0, 0, 200, 200), paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawPaintWithColorSource) {
  const appcode::DlColor colors[2] = {
      appcode::DlColor(0xFFF44336),
      appcode::DlColor(0xFF2196F3),
  };
  const float stops[2] = {0.0, 1.0};
  appcode::DlPaint paint;
  appcode::DisplayListBuilder builder;
  auto clip_bounds = DlRect::MakeWH(300.0, 300.0);
  builder.Save();
  builder.Translate(100, 100);
  builder.ClipRect(clip_bounds, appcode::DlClipOp::kIntersect, false);
  auto linear =
      appcode::DlColorSource::MakeLinear({0.0, 0.0}, {100.0, 100.0}, 2, colors,
                                         stops, appcode::DlTileMode::kRepeat);
  paint.setColorSource(linear);
  builder.DrawPaint(paint);
  builder.Restore();

  builder.Save();
  builder.Translate(500, 100);
  builder.ClipRect(clip_bounds, appcode::DlClipOp::kIntersect, false);
  auto radial = appcode::DlColorSource::MakeRadial(
      {100.0, 100.0}, 100.0, 2, colors, stops, appcode::DlTileMode::kRepeat);
  paint.setColorSource(radial);
  builder.DrawPaint(paint);
  builder.Restore();

  builder.Save();
  builder.Translate(100, 500);
  builder.ClipRect(clip_bounds, appcode::DlClipOp::kIntersect, false);
  auto sweep =
      appcode::DlColorSource::MakeSweep({100.0, 100.0}, 180.0, 270.0, 2, colors,
                                        stops, appcode::DlTileMode::kRepeat);
  paint.setColorSource(sweep);
  builder.DrawPaint(paint);
  builder.Restore();

  builder.Save();
  builder.Translate(500, 500);
  builder.ClipRect(clip_bounds, appcode::DlClipOp::kIntersect, false);
  auto texture = CreateTextureForFixture("table_mountain_nx.png");
  auto image = appcode::DlColorSource::MakeImage(DlImageImpeller::Make(texture),
                                                 appcode::DlTileMode::kRepeat,
                                                 appcode::DlTileMode::kRepeat);
  paint.setColorSource(image);
  builder.DrawPaint(paint);
  builder.Restore();

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanBlendDstOverAndDstCorrectly) {
  appcode::DisplayListBuilder builder;

  {
    builder.SaveLayer(std::nullopt, nullptr);
    builder.Translate(100, 100);
    appcode::DlPaint paint;
    paint.setColor(appcode::DlColor::kRed());
    builder.DrawRect(DlRect::MakeWH(200, 200), paint);
    paint.setColor(appcode::DlColor::kBlue().withAlpha(127));
    paint.setBlendMode(appcode::DlBlendMode::kSrcOver);
    builder.DrawRect(DlRect::MakeWH(200, 200), paint);
    builder.Restore();
  }
  {
    builder.SaveLayer(std::nullopt, nullptr);
    builder.Translate(300, 100);
    appcode::DlPaint paint;
    paint.setColor(appcode::DlColor::kBlue().withAlpha(127));
    builder.DrawRect(DlRect::MakeWH(200, 200), paint);
    paint.setColor(appcode::DlColor::kRed());
    paint.setBlendMode(appcode::DlBlendMode::kDstOver);
    builder.DrawRect(DlRect::MakeWH(200, 200), paint);
    builder.Restore();
  }
  {
    builder.SaveLayer(std::nullopt, nullptr);
    builder.Translate(100, 300);
    appcode::DlPaint paint;
    paint.setColor(appcode::DlColor::kRed());
    builder.DrawRect(DlRect::MakeWH(200, 200), paint);
    paint.setColor(appcode::DlColor::kBlue().withAlpha(127));
    paint.setBlendMode(appcode::DlBlendMode::kSrc);
    builder.DrawRect(DlRect::MakeWH(200, 200), paint);
    builder.Restore();
  }
  {
    builder.SaveLayer(std::nullopt, nullptr);
    builder.Translate(300, 300);
    appcode::DlPaint paint;
    paint.setColor(appcode::DlColor::kBlue().withAlpha(127));
    builder.DrawRect(DlRect::MakeWH(200, 200), paint);
    paint.setColor(appcode::DlColor::kRed());
    paint.setBlendMode(appcode::DlBlendMode::kDst);
    builder.DrawRect(DlRect::MakeWH(200, 200), paint);
    builder.Restore();
  }

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, CanDrawCorrectlyWithColorFilterAndImageFilter) {
  appcode::DisplayListBuilder builder;
  const float green_color_matrix[20] = {
      0, 0, 0, 0, 0,  //
      0, 0, 0, 0, 1,  //
      0, 0, 0, 0, 0,  //
      0, 0, 0, 1, 0,  //
  };
  const float blue_color_matrix[20] = {
      0, 0, 0, 0, 0,  //
      0, 0, 0, 0, 0,  //
      0, 0, 0, 0, 1,  //
      0, 0, 0, 1, 0,  //
  };
  auto green_color_filter =
      appcode::DlColorFilter::MakeMatrix(green_color_matrix);
  auto blue_color_filter =
      appcode::DlColorFilter::MakeMatrix(blue_color_matrix);
  auto blue_image_filter =
      appcode::DlImageFilter::MakeColorFilter(blue_color_filter);

  appcode::DlPaint paint;
  paint.setColor(appcode::DlColor::kRed());
  paint.setColorFilter(green_color_filter);
  paint.setImageFilter(blue_image_filter);
  builder.DrawRect(DlRect::MakeLTRB(100, 100, 500, 500), paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, MaskBlursApplyCorrectlyToColorSources) {
  auto blur_filter = std::make_shared<appcode::DlBlurMaskFilter>(
      appcode::DlBlurStyle::kNormal, 10);

  appcode::DisplayListBuilder builder;

  std::array<appcode::DlColor, 2> colors = {appcode::DlColor::kBlue(),
                                            appcode::DlColor::kGreen()};
  std::array<float, 2> stops = {0, 1};
  auto texture = CreateTextureForFixture("airplane.jpg");
  auto matrix = appcode::DlMatrix::MakeTranslation({-300, -110});
  std::array<std::shared_ptr<appcode::DlColorSource>, 2> color_sources = {
      appcode::DlColorSource::MakeImage(
          DlImageImpeller::Make(texture), appcode::DlTileMode::kRepeat,
          appcode::DlTileMode::kRepeat, appcode::DlImageSampling::kLinear,
          &matrix),
      appcode::DlColorSource::MakeLinear(
          appcode::DlPoint(0, 0), appcode::DlPoint(100, 50), 2, colors.data(),
          stops.data(), appcode::DlTileMode::kClamp),
  };

  builder.Save();
  builder.Translate(0, 100);
  for (const auto& color_source : color_sources) {
    appcode::DlPaint paint;
    paint.setColorSource(color_source);
    paint.setMaskFilter(blur_filter);

    builder.Save();
    builder.Translate(100, 0);
    paint.setDrawStyle(appcode::DlDrawStyle::kFill);
    builder.DrawRoundRect(
        DlRoundRect::MakeRectXY(DlRect::MakeWH(100, 50), 30, 30), paint);

    paint.setDrawStyle(appcode::DlDrawStyle::kStroke);
    paint.setStrokeWidth(10);
    builder.Translate(200, 0);
    builder.DrawRoundRect(
        DlRoundRect::MakeRectXY(DlRect::MakeWH(100, 50), 30, 30), paint);

    builder.Restore();
    builder.Translate(0, 100);
  }
  builder.Restore();

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, DrawShapes) {
  appcode::DisplayListBuilder builder;
  std::vector<appcode::DlStrokeJoin> joins = {
      appcode::DlStrokeJoin::kBevel,
      appcode::DlStrokeJoin::kRound,
      appcode::DlStrokeJoin::kMiter,
  };
  appcode::DlPaint paint =                            //
      appcode::DlPaint()                              //
          .setColor(appcode::DlColor::kWhite())       //
          .setDrawStyle(appcode::DlDrawStyle::kFill)  //
          .setStrokeWidth(10);
  appcode::DlPaint stroke_paint =                       //
      appcode::DlPaint()                                //
          .setColor(appcode::DlColor::kWhite())         //
          .setDrawStyle(appcode::DlDrawStyle::kStroke)  //
          .setStrokeWidth(10);
  DlPath path = DlPath::MakeLine({150, 50}, {160, 50});

  builder.Translate(300, 50);
  builder.Scale(0.8, 0.8);
  for (auto join : joins) {
    paint.setStrokeJoin(join);
    stroke_paint.setStrokeJoin(join);
    builder.DrawRect(DlRect::MakeXYWH(0, 0, 100, 100), paint);
    builder.DrawRect(DlRect::MakeXYWH(0, 150, 100, 100), stroke_paint);
    builder.DrawRoundRect(
        DlRoundRect::MakeRectXY(DlRect::MakeXYWH(150, 0, 100, 100), 30, 30),
        paint);
    builder.DrawRoundRect(
        DlRoundRect::MakeRectXY(DlRect::MakeXYWH(150, 150, 100, 100), 30, 30),
        stroke_paint);
    builder.DrawCircle(DlPoint(350, 50), 50, paint);
    builder.DrawCircle(DlPoint(350, 200), 50, stroke_paint);
    builder.Translate(0, 300);
  }
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, ClipDrawRRectWithNonCircularRadii) {
  appcode::DisplayListBuilder builder;

  appcode::DlPaint fill_paint =                       //
      appcode::DlPaint()                              //
          .setColor(appcode::DlColor::kBlue())        //
          .setDrawStyle(appcode::DlDrawStyle::kFill)  //
          .setStrokeWidth(10);
  appcode::DlPaint stroke_paint =                       //
      appcode::DlPaint()                                //
          .setColor(appcode::DlColor::kGreen())         //
          .setDrawStyle(appcode::DlDrawStyle::kStroke)  //
          .setStrokeWidth(10);

  builder.DrawRoundRect(
      DlRoundRect::MakeRectXY(DlRect::MakeXYWH(500, 100, 300, 300), 120, 40),
      fill_paint);
  builder.DrawRoundRect(
      DlRoundRect::MakeRectXY(DlRect::MakeXYWH(500, 100, 300, 300), 120, 40),
      stroke_paint);

  builder.DrawRoundRect(
      DlRoundRect::MakeRectXY(DlRect::MakeXYWH(100, 500, 300, 300), 40, 120),
      fill_paint);
  builder.DrawRoundRect(
      DlRoundRect::MakeRectXY(DlRect::MakeXYWH(100, 500, 300, 300), 40, 120),
      stroke_paint);

  appcode::DlPaint reference_paint =                  //
      appcode::DlPaint()                              //
          .setColor(appcode::DlColor::kMidGrey())     //
          .setDrawStyle(appcode::DlDrawStyle::kFill)  //
          .setStrokeWidth(10);

  builder.DrawRoundRect(
      DlRoundRect::MakeRectXY(DlRect::MakeXYWH(500, 500, 300, 300), 40, 40),
      reference_paint);
  builder.DrawRoundRect(
      DlRoundRect::MakeRectXY(DlRect::MakeXYWH(100, 100, 300, 300), 120, 120),
      reference_paint);

  appcode::DlPaint clip_fill_paint =                  //
      appcode::DlPaint()                              //
          .setColor(appcode::DlColor::kCyan())        //
          .setDrawStyle(appcode::DlDrawStyle::kFill)  //
          .setStrokeWidth(10);

  builder.Save();
  builder.ClipRoundRect(
      DlRoundRect::MakeRectXY(DlRect::MakeXYWH(900, 100, 300, 300), 120, 40));
  builder.DrawPaint(clip_fill_paint);
  builder.Restore();

  builder.Save();
  builder.ClipRoundRect(
      DlRoundRect::MakeRectXY(DlRect::MakeXYWH(100, 900, 300, 300), 40, 120));
  builder.DrawPaint(clip_fill_paint);
  builder.Restore();

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, DrawVerticesBlendModes) {
  std::vector<const char*> blend_mode_names;
  std::vector<appcode::DlBlendMode> blend_mode_values;
  {
    const std::vector<std::tuple<const char*, appcode::DlBlendMode>> blends = {
        // Pipeline blends (Porter-Duff alpha compositing)
        {"Clear", appcode::DlBlendMode::kClear},
        {"Source", appcode::DlBlendMode::kSrc},
        {"Destination", appcode::DlBlendMode::kDst},
        {"SourceOver", appcode::DlBlendMode::kSrcOver},
        {"DestinationOver", appcode::DlBlendMode::kDstOver},
        {"SourceIn", appcode::DlBlendMode::kSrcIn},
        {"DestinationIn", appcode::DlBlendMode::kDstIn},
        {"SourceOut", appcode::DlBlendMode::kSrcOut},
        {"DestinationOut", appcode::DlBlendMode::kDstOut},
        {"SourceATop", appcode::DlBlendMode::kSrcATop},
        {"DestinationATop", appcode::DlBlendMode::kDstATop},
        {"Xor", appcode::DlBlendMode::kXor},
        {"Plus", appcode::DlBlendMode::kPlus},
        {"Modulate", appcode::DlBlendMode::kModulate},
        // Advanced blends (color component blends)
        {"Screen", appcode::DlBlendMode::kScreen},
        {"Overlay", appcode::DlBlendMode::kOverlay},
        {"Darken", appcode::DlBlendMode::kDarken},
        {"Lighten", appcode::DlBlendMode::kLighten},
        {"ColorDodge", appcode::DlBlendMode::kColorDodge},
        {"ColorBurn", appcode::DlBlendMode::kColorBurn},
        {"HardLight", appcode::DlBlendMode::kHardLight},
        {"SoftLight", appcode::DlBlendMode::kSoftLight},
        {"Difference", appcode::DlBlendMode::kDifference},
        {"Exclusion", appcode::DlBlendMode::kExclusion},
        {"Multiply", appcode::DlBlendMode::kMultiply},
        {"Hue", appcode::DlBlendMode::kHue},
        {"Saturation", appcode::DlBlendMode::kSaturation},
        {"Color", appcode::DlBlendMode::kColor},
        {"Luminosity", appcode::DlBlendMode::kLuminosity},
    };
    assert(blends.size() ==
           static_cast<size_t>(appcode::DlBlendMode::kLastMode) + 1);
    for (const auto& [name, mode] : blends) {
      blend_mode_names.push_back(name);
      blend_mode_values.push_back(mode);
    }
  }

  auto callback = [&]() {
    static int current_blend_index = 3;
    static float dst_alpha = 1;
    static float src_alpha = 1;
    static float color0[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    static float color1[4] = {0.0f, 1.0f, 0.0f, 1.0f};
    static float color2[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    static float src_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
      ImGui::ListBox("Blending mode", &current_blend_index,
                     blend_mode_names.data(), blend_mode_names.size());
      ImGui::SliderFloat("Source alpha", &src_alpha, 0, 1);
      ImGui::ColorEdit4("Color A", color0);
      ImGui::ColorEdit4("Color B", color1);
      ImGui::ColorEdit4("Color C", color2);
      ImGui::ColorEdit4("Source Color", src_color);
      ImGui::SliderFloat("Destination alpha", &dst_alpha, 0, 1);
    }
    ImGui::End();

    std::vector<DlPoint> positions = {DlPoint(100, 300),  //
                                      DlPoint(200, 100),  //
                                      DlPoint(300, 300)};
    std::vector<appcode::DlColor> colors = {
        toColor(color0).modulateOpacity(dst_alpha),
        toColor(color1).modulateOpacity(dst_alpha),
        toColor(color2).modulateOpacity(dst_alpha)};

    auto vertices = appcode::DlVertices::Make(
        appcode::DlVertexMode::kTriangles, 3, positions.data(),
        /*texture_coordinates=*/nullptr, colors.data());

    appcode::DisplayListBuilder builder;
    appcode::DlPaint paint;

    paint.setColor(toColor(src_color).modulateOpacity(src_alpha));
    builder.DrawVertices(vertices, blend_mode_values[current_blend_index],
                         paint);
    return builder.Build();
  };

  ASSERT_TRUE(OpenPlaygroundHere(callback));
}

TEST_P(DisplayListTest, DrawPaintIgnoresMaskFilter) {
  appcode::DisplayListBuilder builder;
  builder.DrawPaint(appcode::DlPaint().setColor(appcode::DlColor::kWhite()));

  auto filter = appcode::DlBlurMaskFilter(appcode::DlBlurStyle::kNormal, 10.0f);
  builder.DrawCircle(DlPoint(300, 300), 200,
                     appcode::DlPaint().setMaskFilter(&filter));

  std::vector<appcode::DlColor> colors = {appcode::DlColor::kGreen(),
                                          appcode::DlColor::kGreen()};
  const float stops[2] = {0.0, 1.0};
  auto linear = appcode::DlColorSource::MakeLinear(
      {100.0, 100.0}, {300.0, 300.0}, 2, colors.data(), stops,
      appcode::DlTileMode::kRepeat);
  appcode::DlPaint blend_paint =
      appcode::DlPaint()           //
          .setColorSource(linear)  //
          .setBlendMode(appcode::DlBlendMode::kScreen);
  builder.DrawPaint(blend_paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(DisplayListTest, DrawMaskBlursThatMightUseSaveLayers) {
  appcode::DisplayListBuilder builder;
  builder.DrawColor(appcode::DlColor::kWhite(), appcode::DlBlendMode::kSrc);
  Vector2 scale = GetContentScale();
  builder.Scale(scale.x, scale.y);

  builder.Save();
  // We need a small transform op to avoid a deferred save
  builder.Translate(1.0f, 1.0f);
  auto solid_filter =
      appcode::DlBlurMaskFilter::Make(appcode::DlBlurStyle::kSolid, 5.0f);
  appcode::DlPaint solid_alpha_paint =
      appcode::DlPaint()                        //
          .setMaskFilter(solid_filter)          //
          .setColor(appcode::DlColor::kBlue())  //
          .setAlpha(0x7f);
  for (int x = 1; x <= 4; x++) {
    for (int y = 1; y <= 4; y++) {
      builder.DrawRect(DlRect::MakeXYWH(x * 100, y * 100, 80, 80),
                       solid_alpha_paint);
    }
  }
  builder.Restore();

  builder.Save();
  builder.Translate(500.0f, 0.0f);
  auto normal_filter =
      appcode::DlBlurMaskFilter::Make(appcode::DlBlurStyle::kNormal, 5.0f);
  auto rotate_if = appcode::DlMatrixImageFilter::Make(
      Matrix::MakeRotationZ(Degrees(10)), appcode::DlImageSampling::kLinear);
  appcode::DlPaint normal_if_paint =
      appcode::DlPaint()                         //
          .setMaskFilter(solid_filter)           //
          .setImageFilter(rotate_if)             //
          .setColor(appcode::DlColor::kGreen())  //
          .setAlpha(0x7f);
  for (int x = 1; x <= 4; x++) {
    for (int y = 1; y <= 4; y++) {
      builder.DrawRect(DlRect::MakeXYWH(x * 100, y * 100, 80, 80),
                       normal_if_paint);
    }
  }
  builder.Restore();

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

}  // namespace testing
}  // namespace impeller
