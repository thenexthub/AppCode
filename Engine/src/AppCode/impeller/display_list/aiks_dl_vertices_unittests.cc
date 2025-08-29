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

#include "display_list/dl_sampling_options.h"
#include "display_list/dl_tile_mode.h"
#include "display_list/dl_vertices.h"
#include "appcode/impeller/display_list/aiks_unittests.h"

#include "appcode/display_list/dl_blend_mode.h"
#include "appcode/display_list/dl_builder.h"
#include "appcode/display_list/dl_color.h"
#include "appcode/display_list/dl_paint.h"
#include "appcode/testing/testing.h"
#include "impeller/display_list/dl_dispatcher.h"
#include "impeller/display_list/dl_image_impeller.h"

namespace impeller {
namespace testing {

using namespace appcode;

namespace {
std::shared_ptr<DlVertices> MakeVertices(
    DlVertexMode mode,
    std::vector<DlPoint> vertices,
    std::vector<uint16_t> indices,
    std::vector<DlPoint> texture_coordinates,
    std::vector<DlColor> colors) {
  DlVertices::Builder::Flags flags(
      {{texture_coordinates.size() > 0, colors.size() > 0}});
  DlVertices::Builder builder(mode, vertices.size(), flags, indices.size());
  if (colors.size() > 0) {
    builder.store_colors(colors.data());
  }
  if (texture_coordinates.size() > 0) {
    builder.store_texture_coordinates(texture_coordinates.data());
  }
  if (indices.size() > 0) {
    builder.store_indices(indices.data());
  }
  builder.store_vertices(vertices.data());
  return builder.build();
}
};  // namespace

// Regression test for https://github.com/appcode/appcode/issues/135441 .
TEST_P(AiksTest, VerticesGeometryUVPositionData) {
  DisplayListBuilder builder;
  DlPaint paint;
  auto image =
      DlImageImpeller::Make(CreateTextureForFixture("table_mountain_nx.png"));
  auto size = image->impeller_texture()->GetSize();

  paint.setColorSource(
      DlColorSource::MakeImage(image, DlTileMode::kClamp, DlTileMode::kClamp));

  std::vector<DlPoint> vertex_coordinates = {
      DlPoint(0, 0),
      DlPoint(size.width, 0),
      DlPoint(0, size.height),
  };
  auto vertices = MakeVertices(DlVertexMode::kTriangleStrip, vertex_coordinates,
                               {0, 1, 2}, {}, {});

  builder.DrawVertices(vertices, DlBlendMode::kSrcOver, paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

// Regression test for https://github.com/appcode/appcode/issues/135441 .
TEST_P(AiksTest, VerticesGeometryUVPositionDataWithTranslate) {
  DisplayListBuilder builder;
  DlPaint paint;
  auto image =
      DlImageImpeller::Make(CreateTextureForFixture("table_mountain_nx.png"));
  auto size = image->impeller_texture()->GetSize();

  DlMatrix matrix = DlMatrix::MakeTranslation({100, 100});
  paint.setColorSource(
      DlColorSource::MakeImage(image, DlTileMode::kClamp, DlTileMode::kClamp,
                               DlImageSampling::kLinear, &matrix));

  std::vector<DlPoint> positions = {
      DlPoint(0, 0),
      DlPoint(size.width, 0),
      DlPoint(0, size.height),
  };
  auto vertices =
      MakeVertices(DlVertexMode::kTriangleStrip, positions, {0, 1, 2}, {}, {});

  builder.DrawVertices(vertices, DlBlendMode::kSrcOver, paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

// Regression test for https://github.com/appcode/appcode/issues/145707
TEST_P(AiksTest, VerticesGeometryColorUVPositionData) {
  DisplayListBuilder builder;
  DlPaint paint;
  auto image =
      DlImageImpeller::Make(CreateTextureForFixture("table_mountain_nx.png"));
  auto size = image->impeller_texture()->GetSize();

  paint.setColorSource(
      DlColorSource::MakeImage(image, DlTileMode::kClamp, DlTileMode::kClamp));

  std::vector<DlPoint> positions = {
      DlPoint(0, 0),           DlPoint(size.width, 0),
      DlPoint(0, size.height), DlPoint(size.width, 0),
      DlPoint(0, 0),           DlPoint(size.width, size.height),
  };
  std::vector<DlColor> colors = {
      DlColor::kRed().withAlpha(128),   DlColor::kBlue().withAlpha(128),
      DlColor::kGreen().withAlpha(128), DlColor::kRed().withAlpha(128),
      DlColor::kBlue().withAlpha(128),  DlColor::kGreen().withAlpha(128),
  };

  auto vertices =
      MakeVertices(DlVertexMode::kTriangles, positions, {}, {}, colors);

  builder.DrawVertices(vertices, DlBlendMode::kDstOver, paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, VerticesGeometryColorUVPositionDataAdvancedBlend) {
  DisplayListBuilder builder;
  DlPaint paint;
  auto image =
      DlImageImpeller::Make(CreateTextureForFixture("table_mountain_nx.png"));
  auto size = image->impeller_texture()->GetSize();

  paint.setColorSource(
      DlColorSource::MakeImage(image, DlTileMode::kClamp, DlTileMode::kClamp));

  std::vector<DlPoint> positions = {
      DlPoint(0, 0),           DlPoint(size.width, 0),
      DlPoint(0, size.height), DlPoint(size.width, 0),
      DlPoint(0, 0),           DlPoint(size.width, size.height),
  };
  std::vector<DlColor> colors = {
      DlColor::kRed().modulateOpacity(0.5),
      DlColor::kBlue().modulateOpacity(0.5),
      DlColor::kGreen().modulateOpacity(0.5),
      DlColor::kRed().modulateOpacity(0.5),
      DlColor::kBlue().modulateOpacity(0.5),
      DlColor::kGreen().modulateOpacity(0.5),
  };

  auto vertices =
      MakeVertices(DlVertexMode::kTriangles, positions, {}, {}, colors);

  builder.DrawVertices(vertices, DlBlendMode::kColorBurn, paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

// Draw a hexagon using triangle fan
TEST_P(AiksTest, CanConvertTriangleFanToTriangles) {
  constexpr Scalar hexagon_radius = 125;
  auto hex_start = Point(200.0, -hexagon_radius + 200.0);
  auto center_to_flat = 1.73 / 2 * hexagon_radius;

  // clang-format off
  std::vector<DlPoint> vertices = {
    DlPoint(hex_start.x, hex_start.y),
    DlPoint(hex_start.x + center_to_flat, hex_start.y + 0.5 * hexagon_radius),
    DlPoint(hex_start.x + center_to_flat, hex_start.y + 1.5 * hexagon_radius),
    DlPoint(hex_start.x + center_to_flat, hex_start.y + 1.5 * hexagon_radius),
    DlPoint(hex_start.x, hex_start.y + 2 * hexagon_radius),
    DlPoint(hex_start.x, hex_start.y + 2 * hexagon_radius),
    DlPoint(hex_start.x - center_to_flat, hex_start.y + 1.5 * hexagon_radius),
    DlPoint(hex_start.x - center_to_flat, hex_start.y + 1.5 * hexagon_radius),
    DlPoint(hex_start.x - center_to_flat, hex_start.y + 0.5 * hexagon_radius)
  };
  // clang-format on
  auto paint = appcode::DlPaint(appcode::DlColor::kDarkGrey());
  auto dl_vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangleFan, vertices.size(), vertices.data(),
      nullptr, nullptr);
  appcode::DisplayListBuilder builder;
  builder.DrawVertices(dl_vertices, appcode::DlBlendMode::kSrcOver, paint);
  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, DrawVerticesSolidColorTrianglesWithoutIndices) {
  // Use negative coordinates and then scale the transform by -1, -1 to make
  // sure coverage is taking the transform into account.
  std::vector<DlPoint> positions = {
      DlPoint(-100, -300),
      DlPoint(-200, -100),
      DlPoint(-300, -300),
  };
  std::vector<appcode::DlColor> colors = {appcode::DlColor::kWhite(),
                                          appcode::DlColor::kGreen(),
                                          appcode::DlColor::kWhite()};

  auto vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangles, 3, positions.data(),
      /*texture_coordinates=*/nullptr, colors.data());

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  paint.setColor(appcode::DlColor::kRed().modulateOpacity(0.5));
  builder.Scale(-1, -1);
  builder.DrawVertices(vertices, appcode::DlBlendMode::kSrcOver, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, DrawVerticesLinearGradientWithoutIndices) {
  std::vector<DlPoint> positions = {
      DlPoint(100, 300),
      DlPoint(200, 100),
      DlPoint(300, 300),
  };

  auto vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangles, 3, positions.data(),
      /*texture_coordinates=*/nullptr, /*colors=*/nullptr);

  std::vector<appcode::DlColor> colors = {appcode::DlColor::kBlue(),
                                          appcode::DlColor::kRed()};
  const float stops[2] = {0.0, 1.0};

  auto linear = appcode::DlColorSource::MakeLinear(
      {100.0, 100.0}, {300.0, 300.0}, 2, colors.data(), stops,
      appcode::DlTileMode::kRepeat);

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  paint.setColorSource(linear);
  builder.DrawVertices(vertices, appcode::DlBlendMode::kSrcOver, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, DrawVerticesLinearGradientWithTextureCoordinates) {
  std::vector<DlPoint> positions = {
      DlPoint(100, 300),
      DlPoint(200, 100),
      DlPoint(300, 300),
  };
  std::vector<DlPoint> texture_coordinates = {
      DlPoint(300, 100),
      DlPoint(100, 200),
      DlPoint(300, 300),
  };

  auto vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangles, 3, positions.data(),
      texture_coordinates.data(), /*colors=*/nullptr);

  std::vector<appcode::DlColor> colors = {appcode::DlColor::kBlue(),
                                          appcode::DlColor::kRed()};
  const float stops[2] = {0.0, 1.0};

  auto linear = appcode::DlColorSource::MakeLinear(
      {100.0, 100.0}, {300.0, 300.0}, 2, colors.data(), stops,
      appcode::DlTileMode::kRepeat);

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  paint.setColorSource(linear);
  builder.DrawVertices(vertices, appcode::DlBlendMode::kSrcOver, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, DrawVerticesImageSourceWithTextureCoordinates) {
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  auto dl_image = DlImageImpeller::Make(texture);
  std::vector<DlPoint> positions = {
      DlPoint(100, 300),
      DlPoint(200, 100),
      DlPoint(300, 300),
  };
  std::vector<DlPoint> texture_coordinates = {
      DlPoint(0, 0),
      DlPoint(100, 200),
      DlPoint(200, 100),
  };

  auto vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangles, 3, positions.data(),
      texture_coordinates.data(), /*colors=*/nullptr);

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  auto image_source = appcode::DlColorSource::MakeImage(
      dl_image, appcode::DlTileMode::kRepeat, appcode::DlTileMode::kRepeat);

  paint.setColorSource(image_source);
  builder.DrawVertices(vertices, appcode::DlBlendMode::kSrcOver, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest,
       DrawVerticesImageSourceWithTextureCoordinatesAndColorBlending) {
  auto texture = CreateTextureForFixture("embarcadero.jpg");
  auto dl_image = DlImageImpeller::Make(texture);
  std::vector<DlPoint> positions = {
      DlPoint(100, 300),
      DlPoint(200, 100),
      DlPoint(300, 300),
  };
  std::vector<appcode::DlColor> colors = {appcode::DlColor::kWhite(),
                                          appcode::DlColor::kGreen(),
                                          appcode::DlColor::kWhite()};
  std::vector<DlPoint> texture_coordinates = {
      DlPoint(0, 0),
      DlPoint(100, 200),
      DlPoint(200, 100),
  };

  auto vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangles, 3, positions.data(),
      texture_coordinates.data(), colors.data());

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  auto image_source = appcode::DlColorSource::MakeImage(
      dl_image, appcode::DlTileMode::kRepeat, appcode::DlTileMode::kRepeat);

  paint.setColorSource(image_source);
  builder.DrawVertices(vertices, appcode::DlBlendMode::kModulate, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, DrawVerticesSolidColorTrianglesWithIndices) {
  std::vector<DlPoint> positions = {
      DlPoint(100, 300),
      DlPoint(200, 100),
      DlPoint(300, 300),
      DlPoint(200, 500),
  };
  std::vector<uint16_t> indices = {0, 1, 2, 0, 2, 3};

  auto vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangles, positions.size(), positions.data(),
      /*texture_coordinates=*/nullptr, /*colors=*/nullptr, indices.size(),
      indices.data());

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;

  paint.setColor(appcode::DlColor::kRed());
  builder.DrawVertices(vertices, appcode::DlBlendMode::kSrcOver, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, DrawVerticesPremultipliesColors) {
  std::vector<DlPoint> positions = {
      DlPoint(100, 300),
      DlPoint(200, 100),
      DlPoint(300, 300),
      DlPoint(200, 500),
  };
  auto color = appcode::DlColor::kBlue().withAlpha(0x99);
  std::vector<uint16_t> indices = {0, 1, 2, 0, 2, 3};
  std::vector<appcode::DlColor> colors = {color, color, color, color};

  auto vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangles, positions.size(), positions.data(),
      /*texture_coordinates=*/nullptr, colors.data(), indices.size(),
      indices.data());

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;
  paint.setBlendMode(appcode::DlBlendMode::kSrcOver);
  paint.setColor(appcode::DlColor::kRed());

  builder.DrawRect(DlRect::MakeLTRB(0, 0, 400, 400), paint);
  builder.DrawVertices(vertices, appcode::DlBlendMode::kDst, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, DrawVerticesWithInvalidIndices) {
  std::vector<DlPoint> positions = {
      DlPoint(100, 300),
      DlPoint(200, 100),
      DlPoint(300, 300),
      DlPoint(200, 500),
  };
  std::vector<uint16_t> indices = {0, 1, 2, 0, 2, 3, 99, 100, 101};

  auto vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangles, positions.size(), positions.data(),
      /*texture_coordinates=*/nullptr, /*colors=*/nullptr, indices.size(),
      indices.data());

  EXPECT_EQ(vertices->GetBounds(), DlRect::MakeLTRB(100, 100, 300, 500));

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;
  paint.setBlendMode(appcode::DlBlendMode::kSrcOver);
  paint.setColor(appcode::DlColor::kRed());

  builder.DrawRect(DlRect::MakeLTRB(0, 0, 400, 400), paint);
  builder.DrawVertices(vertices, appcode::DlBlendMode::kSrc, paint);

  AiksContext renderer(GetContext(), nullptr);
  std::shared_ptr<Texture> image =
      DisplayListToTexture(builder.Build(), {1024, 768}, renderer);
  EXPECT_TRUE(image);
}

// All four vertices should form a solid red rectangle with no gaps.
// The blue rectangle drawn under them should not be visible.
TEST_P(AiksTest, DrawVerticesTextureCoordinatesWithFragmentShader) {
  std::vector<DlPoint> positions_lt = {
      DlPoint(0, 0),    //
      DlPoint(50, 0),   //
      DlPoint(0, 50),   //
      DlPoint(50, 50),  //
  };

  auto vertices_lt = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangleStrip, positions_lt.size(),
      positions_lt.data(),
      /*texture_coordinates=*/positions_lt.data(), /*colors=*/nullptr,
      /*index_count=*/0,
      /*indices=*/nullptr);

  std::vector<DlPoint> positions_rt = {
      DlPoint(50, 0),    //
      DlPoint(100, 0),   //
      DlPoint(50, 50),   //
      DlPoint(100, 50),  //
  };

  auto vertices_rt = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangleStrip, positions_rt.size(),
      positions_rt.data(),
      /*texture_coordinates=*/positions_rt.data(), /*colors=*/nullptr,
      /*index_count=*/0,
      /*indices=*/nullptr);

  std::vector<DlPoint> positions_lb = {
      DlPoint(0, 50),    //
      DlPoint(50, 50),   //
      DlPoint(0, 100),   //
      DlPoint(50, 100),  //
  };

  auto vertices_lb = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangleStrip, positions_lb.size(),
      positions_lb.data(),
      /*texture_coordinates=*/positions_lb.data(), /*colors=*/nullptr,
      /*index_count=*/0,
      /*indices=*/nullptr);

  std::vector<DlPoint> positions_rb = {
      DlPoint(50, 50),    //
      DlPoint(100, 50),   //
      DlPoint(50, 100),   //
      DlPoint(100, 100),  //
  };

  auto vertices_rb = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangleStrip, positions_rb.size(),
      positions_rb.data(),
      /*texture_coordinates=*/positions_rb.data(), /*colors=*/nullptr,
      /*index_count=*/0,
      /*indices=*/nullptr);

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;
  appcode::DlPaint rect_paint;
  rect_paint.setColor(DlColor::kBlue());

  auto runtime_stages =
      OpenAssetAsRuntimeStage("runtime_stage_simple.frag.iplr");

  auto runtime_stage =
      runtime_stages[PlaygroundBackendToRuntimeStageBackend(GetBackend())];
  ASSERT_TRUE(runtime_stage);

  auto runtime_effect = DlRuntimeEffect::MakeImpeller(runtime_stage);
  auto uniform_data = std::make_shared<std::vector<uint8_t>>();
  auto color_source = appcode::DlColorSource::MakeRuntimeEffect(
      runtime_effect, {}, uniform_data);

  paint.setColorSource(color_source);

  builder.Scale(GetContentScale().x, GetContentScale().y);
  builder.Save();
  builder.DrawRect(DlRect::MakeLTRB(0, 0, 100, 100), rect_paint);
  builder.DrawVertices(vertices_lt, appcode::DlBlendMode::kSrcOver, paint);
  builder.DrawVertices(vertices_rt, appcode::DlBlendMode::kSrcOver, paint);
  builder.DrawVertices(vertices_lb, appcode::DlBlendMode::kSrcOver, paint);
  builder.DrawVertices(vertices_rb, appcode::DlBlendMode::kSrcOver, paint);
  builder.Restore();

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

// The vertices should form a solid red rectangle with no gaps.
// The blue rectangle drawn under them should not be visible.
TEST_P(AiksTest,
       DrawVerticesTextureCoordinatesWithFragmentShaderNonZeroOrigin) {
  std::vector<DlPoint> positions_lt = {
      DlPoint(200, 200),  //
      DlPoint(250, 200),  //
      DlPoint(200, 250),  //
      DlPoint(250, 250),  //
  };

  auto vertices = appcode::DlVertices::Make(
      appcode::DlVertexMode::kTriangleStrip, positions_lt.size(),
      positions_lt.data(),
      /*texture_coordinates=*/positions_lt.data(), /*colors=*/nullptr,
      /*index_count=*/0,
      /*indices=*/nullptr);

  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;
  appcode::DlPaint rect_paint;
  rect_paint.setColor(DlColor::kBlue());

  auto runtime_stages =
      OpenAssetAsRuntimeStage("runtime_stage_position.frag.iplr");

  auto runtime_stage =
      runtime_stages[PlaygroundBackendToRuntimeStageBackend(GetBackend())];
  ASSERT_TRUE(runtime_stage);

  auto runtime_effect = DlRuntimeEffect::MakeImpeller(runtime_stage);
  auto rect_data = std::vector<Rect>{Rect::MakeLTRB(200, 200, 250, 250)};

  auto uniform_data = std::make_shared<std::vector<uint8_t>>();
  uniform_data->resize(rect_data.size() * sizeof(Rect));
  memcpy(uniform_data->data(), rect_data.data(), uniform_data->size());

  auto color_source = appcode::DlColorSource::MakeRuntimeEffect(
      runtime_effect, {}, uniform_data);

  paint.setColorSource(color_source);

  builder.Scale(GetContentScale().x, GetContentScale().y);
  builder.DrawRect(DlRect::MakeLTRB(200, 200, 250, 250), rect_paint);
  builder.DrawVertices(vertices, appcode::DlBlendMode::kSrcOver, paint);

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

}  // namespace testing
}  // namespace impeller
