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

#ifndef APPCODE_FLOW_TESTING_MOCK_EMBEDDER_H_
#define APPCODE_FLOW_TESTING_MOCK_EMBEDDER_H_

#include "appcode/flow/embedded_views.h"

namespace appcode {
namespace testing {

class MockViewEmbedder : public ExternalViewEmbedder {
 public:
  MockViewEmbedder();

  ~MockViewEmbedder();

  void AddCanvas(DlCanvas* canvas);

  // |ExternalViewEmbedder|
  DlCanvas* GetRootCanvas() override;

  // |ExternalViewEmbedder|
  void CancelFrame() override;

  // |ExternalViewEmbedder|
  void BeginFrame(GrDirectContext* context,
                  const fml::RefPtr<fml::RasterThreadMerger>&
                      raster_thread_merger) override;

  // |ExternalViewEmbedder|
  void PrepareFlutterView(SkISize frame_size,
                          double device_pixel_ratio) override;

  // |ExternalViewEmbedder|
  void PrerollCompositeEmbeddedView(
      int64_t view_id,
      std::unique_ptr<EmbeddedViewParams> params) override;

  // |ExternalViewEmbedder|
  DlCanvas* CompositeEmbeddedView(int64_t view_id) override;

  std::vector<int64_t> prerolled_views() const { return prerolled_views_; }
  std::vector<int64_t> painted_views() const { return painted_views_; }

 private:
  std::deque<DlCanvas*> contexts_;
  std::vector<int64_t> prerolled_views_;
  std::vector<int64_t> painted_views_;
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_FLOW_TESTING_MOCK_EMBEDDER_H_
