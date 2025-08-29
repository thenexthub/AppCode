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

#include <unordered_map>

#include "appcode/display_list/dl_tile_mode.h"
#include "appcode/display_list/effects/dl_image_filter.h"
#include "appcode/display_list/geometry/dl_geometry_types.h"
#include "appcode/testing/testing.h"
#include "gtest/gtest.h"
#include "impeller/core/formats.h"
#include "impeller/core/texture_descriptor.h"
#include "impeller/display_list/aiks_unittests.h"
#include "impeller/display_list/canvas.h"
#include "impeller/display_list/dl_vertices_geometry.h"
#include "impeller/geometry/geometry_asserts.h"
#include "impeller/playground/playground.h"
#include "impeller/renderer/render_target.h"

namespace impeller {
namespace testing {

std::unique_ptr<Canvas> CreateTestCanvas(
    ContentContext& context,
    std::optional<Rect> cull_rect = std::nullopt,
    bool requires_readback = false) {
  TextureDescriptor onscreen_desc;
  onscreen_desc.size = {100, 100};
  onscreen_desc.format =
      context.GetDeviceCapabilities().GetDefaultColorFormat();
  onscreen_desc.usage = TextureUsage::kRenderTarget;
  onscreen_desc.storage_mode = StorageMode::kDevicePrivate;
  onscreen_desc.sample_count = SampleCount::kCount1;
  std::shared_ptr<Texture> onscreen =
      context.GetContext()->GetResourceAllocator()->CreateTexture(
          onscreen_desc);

  ColorAttachment color0;
  color0.load_action = LoadAction::kClear;
  if (context.GetContext()->GetCapabilities()->SupportsOffscreenMSAA()) {
    TextureDescriptor onscreen_msaa_desc = onscreen_desc;
    onscreen_msaa_desc.sample_count = SampleCount::kCount4;
    onscreen_msaa_desc.storage_mode = StorageMode::kDeviceTransient;
    onscreen_msaa_desc.type = TextureType::kTexture2DMultisample;

    std::shared_ptr<Texture> onscreen_msaa =
        context.GetContext()->GetResourceAllocator()->CreateTexture(
            onscreen_msaa_desc);
    color0.resolve_texture = onscreen;
    color0.texture = onscreen_msaa;
    color0.store_action = StoreAction::kMultisampleResolve;
  } else {
    color0.texture = onscreen;
  }

  RenderTarget render_target;
  render_target.SetColorAttachment(color0, 0);

  if (cull_rect.has_value()) {
    return std::make_unique<Canvas>(
        context, render_target, /*is_onscreen=*/false,
        /*requires_readback=*/requires_readback, cull_rect.value());
  }
  return std::make_unique<Canvas>(context, render_target, /*is_onscreen=*/false,
                                  /*requires_readback=*/requires_readback);
}

TEST_P(AiksTest, TransformMultipliesCorrectly) {
  ContentContext context(GetContext(), nullptr);
  auto canvas = CreateTestCanvas(context);

  ASSERT_MATRIX_NEAR(canvas->GetCurrentTransform(), Matrix());

  // clang-format off
  canvas->Translate(Vector3(100, 200));
  ASSERT_MATRIX_NEAR(
    canvas->GetCurrentTransform(),
    Matrix(  1,   0,   0,   0,
             0,   1,   0,   0,
             0,   0,   1,   0,
           100, 200,   0,   1));

  canvas->Rotate(Radians(kPiOver2));
  ASSERT_MATRIX_NEAR(
    canvas->GetCurrentTransform(),
    Matrix(  0,   1,   0,   0,
            -1,   0,   0,   0,
             0,   0,   1,   0,
           100, 200,   0,   1));

  canvas->Scale(Vector3(2, 3));
  ASSERT_MATRIX_NEAR(
    canvas->GetCurrentTransform(),
    Matrix(  0,   2,   0,   0,
            -3,   0,   0,   0,
             0,   0,   0,   0,
           100, 200,   0,   1));

  canvas->Translate(Vector3(100, 200));
  ASSERT_MATRIX_NEAR(
    canvas->GetCurrentTransform(),
    Matrix(   0,   2,   0,   0,
             -3,   0,   0,   0,
              0,   0,   0,   0,
           -500, 400,   0,   1));
  // clang-format on
}

TEST_P(AiksTest, CanvasCanPushPopCTM) {
  ContentContext context(GetContext(), nullptr);
  auto canvas = CreateTestCanvas(context);

  ASSERT_EQ(canvas->GetSaveCount(), 1u);
  ASSERT_EQ(canvas->Restore(), false);

  canvas->Translate(Size{100, 100});
  canvas->Save(10);
  ASSERT_EQ(canvas->GetSaveCount(), 2u);
  ASSERT_MATRIX_NEAR(canvas->GetCurrentTransform(),
                     Matrix::MakeTranslation({100.0, 100.0, 0.0}));
  ASSERT_TRUE(canvas->Restore());
  ASSERT_EQ(canvas->GetSaveCount(), 1u);
  ASSERT_MATRIX_NEAR(canvas->GetCurrentTransform(),
                     Matrix::MakeTranslation({100.0, 100.0, 0.0}));
}

TEST_P(AiksTest, CanvasCTMCanBeUpdated) {
  ContentContext context(GetContext(), nullptr);
  auto canvas = CreateTestCanvas(context);

  Matrix identity;
  ASSERT_MATRIX_NEAR(canvas->GetCurrentTransform(), identity);
  canvas->Translate(Size{100, 100});
  ASSERT_MATRIX_NEAR(canvas->GetCurrentTransform(),
                     Matrix::MakeTranslation({100.0, 100.0, 0.0}));
}

TEST_P(AiksTest, BackdropCountDownNormal) {
  ContentContext context(GetContext(), nullptr);
  if (!context.GetDeviceCapabilities().SupportsFramebufferFetch()) {
    GTEST_SKIP() << "Test requires device with framebuffer fetch";
  }
  auto canvas = CreateTestCanvas(context, Rect::MakeLTRB(0, 0, 100, 100),
                                 /*requires_readback=*/true);
  // 3 backdrop filters
  canvas->SetBackdropData({}, 3);

  auto blur =
      appcode::DlImageFilter::MakeBlur(4, 4, appcode::DlTileMode::kClamp);
  appcode::DlRect rect = appcode::DlRect::MakeLTRB(0, 0, 50, 50);

  EXPECT_TRUE(canvas->RequiresReadback());
  canvas->DrawRect(rect, {.color = Color::Azure()});
  canvas->SaveLayer({}, rect, blur.get(),
                    ContentBoundsPromise::kContainsContents,
                    /*total_content_depth=*/1);
  canvas->Restore();
  EXPECT_TRUE(canvas->RequiresReadback());

  canvas->SaveLayer({}, rect, blur.get(),
                    ContentBoundsPromise::kContainsContents,
                    /*total_content_depth=*/1);
  canvas->Restore();
  EXPECT_TRUE(canvas->RequiresReadback());

  canvas->SaveLayer({}, rect, blur.get(),
                    ContentBoundsPromise::kContainsContents,
                    /*total_content_depth=*/1);
  canvas->Restore();
  EXPECT_FALSE(canvas->RequiresReadback());
}

TEST_P(AiksTest, BackdropCountDownBackdropId) {
  ContentContext context(GetContext(), nullptr);
  if (!context.GetDeviceCapabilities().SupportsFramebufferFetch()) {
    GTEST_SKIP() << "Test requires device with framebuffer fetch";
  }
  auto canvas = CreateTestCanvas(context, Rect::MakeLTRB(0, 0, 100, 100),
                                 /*requires_readback=*/true);
  // 3 backdrop filters all with same id.
  std::unordered_map<int64_t, BackdropData> data;
  data[1] = BackdropData{.backdrop_count = 3};
  canvas->SetBackdropData(data, 3);

  auto blur =
      appcode::DlImageFilter::MakeBlur(4, 4, appcode::DlTileMode::kClamp);

  EXPECT_TRUE(canvas->RequiresReadback());
  canvas->DrawRect(appcode::DlRect::MakeLTRB(0, 0, 50, 50),
                   {.color = Color::Azure()});
  canvas->SaveLayer({}, std::nullopt, blur.get(),
                    ContentBoundsPromise::kContainsContents,
                    /*total_content_depth=*/1, /*can_distribute_opacity=*/false,
                    /*backdrop_id=*/1);
  canvas->Restore();
  EXPECT_FALSE(canvas->RequiresReadback());

  canvas->SaveLayer({}, std::nullopt, blur.get(),
                    ContentBoundsPromise::kContainsContents,
                    /*total_content_depth=*/1, /*can_distribute_opacity=*/false,
                    /*backdrop_id=*/1);
  canvas->Restore();
  EXPECT_FALSE(canvas->RequiresReadback());

  canvas->SaveLayer({}, std::nullopt, blur.get(),
                    ContentBoundsPromise::kContainsContents,
                    /*total_content_depth=*/1, /*can_distribute_opacity=*/false,
                    /*backdrop_id=*/1);
  canvas->Restore();
  EXPECT_FALSE(canvas->RequiresReadback());
}

TEST_P(AiksTest, BackdropCountDownBackdropIdMixed) {
  ContentContext context(GetContext(), nullptr);
  if (!context.GetDeviceCapabilities().SupportsFramebufferFetch()) {
    GTEST_SKIP() << "Test requires device with framebuffer fetch";
  }
  auto canvas = CreateTestCanvas(context, Rect::MakeLTRB(0, 0, 100, 100),
                                 /*requires_readback=*/true);
  // 3 backdrop filters, 2 with same id.
  std::unordered_map<int64_t, BackdropData> data;
  data[1] = BackdropData{.backdrop_count = 2};
  canvas->SetBackdropData(data, 3);

  auto blur =
      appcode::DlImageFilter::MakeBlur(4, 4, appcode::DlTileMode::kClamp);

  EXPECT_TRUE(canvas->RequiresReadback());
  canvas->DrawRect(appcode::DlRect::MakeLTRB(0, 0, 50, 50),
                   {.color = Color::Azure()});
  canvas->SaveLayer({}, std::nullopt, blur.get(),
                    ContentBoundsPromise::kContainsContents, 1, false);
  canvas->Restore();
  EXPECT_TRUE(canvas->RequiresReadback());

  canvas->SaveLayer({}, std::nullopt, blur.get(),
                    ContentBoundsPromise::kContainsContents, 1, false, 1);
  canvas->Restore();
  EXPECT_FALSE(canvas->RequiresReadback());

  canvas->SaveLayer({}, std::nullopt, blur.get(),
                    ContentBoundsPromise::kContainsContents, 1, false, 1);
  canvas->Restore();
  EXPECT_FALSE(canvas->RequiresReadback());
}

// We only know the total number of backdrop filters, not the number of backdrop
// filters in the root pass. If we reach a count of 0 while in a nested
// saveLayer, we should not restore to the onscreen.
TEST_P(AiksTest, BackdropCountDownWithNestedSaveLayers) {
  ContentContext context(GetContext(), nullptr);
  if (!context.GetDeviceCapabilities().SupportsFramebufferFetch()) {
    GTEST_SKIP() << "Test requires device with framebuffer fetch";
  }
  auto canvas = CreateTestCanvas(context, Rect::MakeLTRB(0, 0, 100, 100),
                                 /*requires_readback=*/true);

  canvas->SetBackdropData({}, 2);

  auto blur =
      appcode::DlImageFilter::MakeBlur(4, 4, appcode::DlTileMode::kClamp);

  EXPECT_TRUE(canvas->RequiresReadback());
  canvas->DrawRect(appcode::DlRect::MakeLTRB(0, 0, 50, 50),
                   {.color = Color::Azure()});
  canvas->SaveLayer({}, std::nullopt, blur.get(),
                    ContentBoundsPromise::kContainsContents,
                    /*total_content_depth=*/3);

  // This filter is nested in the first saveLayer. We cannot restore to onscreen
  // here.
  canvas->SaveLayer({}, std::nullopt, blur.get(),
                    ContentBoundsPromise::kContainsContents,
                    /*total_content_depth=*/1);
  canvas->Restore();
  EXPECT_TRUE(canvas->RequiresReadback());

  canvas->Restore();
  EXPECT_TRUE(canvas->RequiresReadback());
}

TEST_P(AiksTest, DrawVerticesLinearGradientWithEmptySize) {
  RenderCallback callback = [&](RenderTarget& render_target) {
    ContentContext context(GetContext(), nullptr);
    Canvas canvas(context, render_target, true, false);

    std::vector<appcode::DlPoint> vertex_coordinates = {
        appcode::DlPoint(0, 0),
        appcode::DlPoint(600, 0),
        appcode::DlPoint(0, 600),
    };
    std::vector<appcode::DlPoint> texture_coordinates = {
        appcode::DlPoint(0, 0),
        appcode::DlPoint(500, 0),
        appcode::DlPoint(0, 500),
    };
    std::vector<uint16_t> indices = {0, 1, 2};
    appcode::DlVertices::Builder vertices_builder(
        appcode::DlVertexMode::kTriangleStrip, vertex_coordinates.size(),
        appcode::DlVertices::Builder::kHasTextureCoordinates, indices.size());
    vertices_builder.store_vertices(vertex_coordinates.data());
    vertices_builder.store_indices(indices.data());
    vertices_builder.store_texture_coordinates(texture_coordinates.data());
    auto vertices = vertices_builder.build();

    // The start and end points of the gradient form an empty rectangle.
    std::vector<appcode::DlColor> colors = {appcode::DlColor::kBlue(),
                                            appcode::DlColor::kRed()};
    std::vector<Scalar> stops = {0.0, 1.0};
    auto gradient = appcode::DlColorSource::MakeLinear(
        {0, 0}, {0, 600}, 2, colors.data(), stops.data(),
        appcode::DlTileMode::kClamp);

    Paint paint;
    paint.color_source = gradient.get();
    canvas.DrawVertices(std::make_shared<DlVerticesGeometry>(vertices, context),
                        BlendMode::kSrcOver, paint);

    canvas.EndReplay();
    return true;
  };

  ASSERT_TRUE(Playground::OpenPlaygroundHere(callback));
}

TEST_P(AiksTest, DrawVerticesWithEmptyTextureCoordinates) {
  auto runtime_stages =
      OpenAssetAsRuntimeStage("runtime_stage_simple.frag.iplr");

  auto runtime_stage =
      runtime_stages[PlaygroundBackendToRuntimeStageBackend(GetBackend())];
  ASSERT_TRUE(runtime_stage);

  auto runtime_effect = appcode::DlRuntimeEffect::MakeImpeller(runtime_stage);
  auto uniform_data = std::make_shared<std::vector<uint8_t>>();
  auto color_source = appcode::DlColorSource::MakeRuntimeEffect(
      runtime_effect, {}, uniform_data);

  RenderCallback callback = [&](RenderTarget& render_target) {
    ContentContext context(GetContext(), nullptr);
    Canvas canvas(context, render_target, true, false);

    std::vector<appcode::DlPoint> vertex_coordinates = {
        appcode::DlPoint(100, 100),
        appcode::DlPoint(300, 100),
        appcode::DlPoint(100, 300),
    };
    // The bounding box of the texture coordinates is empty.
    std::vector<appcode::DlPoint> texture_coordinates = {
        appcode::DlPoint(0, 0),
        appcode::DlPoint(0, 100),
        appcode::DlPoint(0, 0),
    };
    std::vector<uint16_t> indices = {0, 1, 2};
    appcode::DlVertices::Builder vertices_builder(
        appcode::DlVertexMode::kTriangleStrip, vertex_coordinates.size(),
        appcode::DlVertices::Builder::kHasTextureCoordinates, indices.size());
    vertices_builder.store_vertices(vertex_coordinates.data());
    vertices_builder.store_indices(indices.data());
    vertices_builder.store_texture_coordinates(texture_coordinates.data());
    auto vertices = vertices_builder.build();

    Paint paint;
    paint.color_source = color_source.get();
    canvas.DrawVertices(std::make_shared<DlVerticesGeometry>(vertices, context),
                        BlendMode::kSrcOver, paint);

    canvas.EndReplay();
    return true;
  };

  ASSERT_TRUE(Playground::OpenPlaygroundHere(callback));
}

TEST_P(AiksTest, SupportsBlitToOnscreen) {
  ContentContext context(GetContext(), nullptr);
  auto canvas = CreateTestCanvas(context, Rect::MakeLTRB(0, 0, 100, 100),
                                 /*requires_readback=*/true);

  if (GetBackend() != PlaygroundBackend::kMetal) {
    EXPECT_FALSE(canvas->SupportsBlitToOnscreen());
  } else {
    EXPECT_TRUE(canvas->SupportsBlitToOnscreen());
  }
}

}  // namespace testing
}  // namespace impeller
