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

#include "appcode/shell/platform/fuchsia/appcode/canvas_spy.h"

#include "gtest/gtest.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace appcode {
namespace testing {

TEST(CanvasSpyTest, DrawingIsTracked) {
  SkPictureRecorder picture_recorder;
  SkCanvas* canvas = picture_recorder.beginRecording(100, 100);
  CanvasSpy canvas_spy = CanvasSpy(canvas);
  DlCanvas* spy = canvas_spy.GetSpyingCanvas();

  ASSERT_FALSE(canvas_spy.DidDrawIntoCanvas());

  spy->Translate(128, 128);
  spy->Clear(DlColor::kTransparent());
  ASSERT_FALSE(canvas_spy.DidDrawIntoCanvas());

  DlPaint paint;
  spy->DrawCircle(DlPoint(0, 0), 60, paint);
  ASSERT_TRUE(canvas_spy.DidDrawIntoCanvas());
}

TEST(CanvasSpyTest, SpiedCanvasIsDrawing) {
  auto actual_surface =
      SkSurfaces::Raster(SkImageInfo::MakeN32Premul(100, 100));
  SkCanvas* actual_canvas = actual_surface->getCanvas();

  auto expected_surface =
      SkSurfaces::Raster(SkImageInfo::MakeN32Premul(100, 100));
  SkCanvas* expected_canvas = expected_surface->getCanvas();

  CanvasSpy canvas_spy = CanvasSpy(actual_canvas);
  SkCanvas* spy = canvas_spy.GetRawSpyingCanvas();

  SkNWayCanvas multi_canvas = SkNWayCanvas(100, 100);
  multi_canvas.addCanvas(spy);
  multi_canvas.addCanvas(expected_canvas);

  multi_canvas.clipRect(SkRect::MakeWH(100, 100));
  multi_canvas.clear(SK_ColorRED);
  multi_canvas.scale(.5, .5);
  multi_canvas.clipRect(SkRect::MakeWH(100, 100));
  multi_canvas.clear(SK_ColorBLUE);

  ASSERT_TRUE(canvas_spy.DidDrawIntoCanvas());

  SkPixmap actual;
  SkPixmap expected;

  ASSERT_TRUE(actual_surface->peekPixels(&actual));
  ASSERT_TRUE(expected_surface->peekPixels(&expected));

  const auto size = actual.rowBytes() * actual.height();
  ASSERT_NE(size, 0u);

  ASSERT_EQ(::memcmp(actual.addr(), expected.addr(), size), 0);
}
}  // namespace testing
}  // namespace appcode
