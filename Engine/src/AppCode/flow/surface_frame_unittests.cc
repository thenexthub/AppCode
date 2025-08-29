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

#define FML_USED_ON_EMBEDDER

#include "appcode/flow/surface_frame.h"
#include "appcode/testing/testing.h"

namespace appcode {

TEST(FlowTest, SurfaceFrameDoesNotSubmitInDtor) {
  SurfaceFrame::FramebufferInfo framebuffer_info;
  auto callback = [](const SurfaceFrame&) {
    EXPECT_FALSE(true);
    return true;
  };
  auto encode_callback = [](const SurfaceFrame&, DlCanvas*) {
    EXPECT_FALSE(true);
    return true;
  };

  auto surface_frame = std::make_unique<SurfaceFrame>(
      /*surface=*/nullptr,
      /*framebuffer_info=*/framebuffer_info,
      /*encode_callback=*/encode_callback,
      /*submit_callback=*/callback,
      /*frame_size=*/SkISize::Make(800, 600));
  surface_frame.reset();
}

TEST(FlowTest, SurfaceFrameDoesNotHaveEmptyCanvas) {
  SurfaceFrame::FramebufferInfo framebuffer_info;
  auto callback = [](const SurfaceFrame&, DlCanvas*) { return true; };
  auto submit_callback = [](const SurfaceFrame&) { return true; };
  SurfaceFrame frame(
      /*surface=*/nullptr,
      /*framebuffer_info=*/framebuffer_info,
      /*encode_callback=*/callback,
      /*submit_callback=*/submit_callback,
      /*frame_size=*/SkISize::Make(800, 600),
      /*context_result=*/nullptr,
      /*display_list_fallback=*/true);

  EXPECT_FALSE(frame.Canvas()->GetLocalClipCoverage().IsEmpty());
  EXPECT_FALSE(frame.Canvas()->QuickReject(DlRect::MakeLTRB(10, 10, 50, 50)));
}

TEST(FlowTest, SurfaceFrameDoesNotPrepareRtree) {
  SurfaceFrame::FramebufferInfo framebuffer_info;
  auto callback = [](const SurfaceFrame&, DlCanvas*) { return true; };
  auto submit_callback = [](const SurfaceFrame&) { return true; };
  auto surface_frame = std::make_unique<SurfaceFrame>(
      /*surface=*/nullptr,
      /*framebuffer_info=*/framebuffer_info,
      /*encode_callback=*/callback,
      /*submit_callback=*/submit_callback,
      /*frame_size=*/SkISize::Make(800, 600),
      /*context_result=*/nullptr,
      /*display_list_fallback=*/true);
  surface_frame->Canvas()->DrawRect(DlRect::MakeWH(100, 100), DlPaint());
  EXPECT_FALSE(surface_frame->BuildDisplayList()->has_rtree());
}

}  // namespace appcode
