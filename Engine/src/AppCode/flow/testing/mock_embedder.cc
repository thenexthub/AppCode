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

#include "appcode/flow/testing/mock_embedder.h"

namespace appcode {
namespace testing {

MockViewEmbedder::MockViewEmbedder() = default;

MockViewEmbedder::~MockViewEmbedder() = default;

void MockViewEmbedder::AddCanvas(DlCanvas* canvas) {
  contexts_.emplace_back(canvas);
}

// |ExternalViewEmbedder|
DlCanvas* MockViewEmbedder::GetRootCanvas() {
  return nullptr;
}

// |ExternalViewEmbedder|
void MockViewEmbedder::CancelFrame() {}

// |ExternalViewEmbedder|
void MockViewEmbedder::BeginFrame(
    GrDirectContext* context,
    const fml::RefPtr<fml::RasterThreadMerger>& raster_thread_merger) {}

// |ExternalViewEmbedder|
void MockViewEmbedder::PrepareappcodeView(SkISize frame_size,
                                          double device_pixel_ratio) {}

// |ExternalViewEmbedder|
void MockViewEmbedder::PrerollCompositeEmbeddedView(
    int64_t view_id,
    std::unique_ptr<EmbeddedViewParams> params) {
  prerolled_views_.emplace_back(view_id);
}

// |ExternalViewEmbedder|
DlCanvas* MockViewEmbedder::CompositeEmbeddedView(int64_t view_id) {
  painted_views_.emplace_back(view_id);
  DlCanvas* canvas = contexts_.front();
  contexts_.pop_front();
  return canvas;
}

}  // namespace testing
}  // namespace appcode
