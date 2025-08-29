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

#ifndef APPCODE_FLOW_TESTING_MOCK_TEXTURE_H_
#define APPCODE_FLOW_TESTING_MOCK_TEXTURE_H_

#include <ostream>
#include <vector>

#include "appcode/common/graphics/texture.h"
#include "appcode/testing/assertions_skia.h"

namespace appcode {
namespace testing {

// Mock implementation of the |Texture| interface that does not interact with
// the GPU.  It simply records the list of various calls made so the test can
// later verify them against expected data.
class MockTexture : public Texture {
 public:
  static sk_sp<DlImage> MakeTestTexture(int w, int h, int checker_size);

  explicit MockTexture(int64_t textureId,
                       const sk_sp<DlImage>& texture = nullptr);

  // Called from raster thread.
  void Paint(PaintContext& context,
             const DlRect& bounds,
             bool freeze,
             const DlImageSampling sampling) override;

  void OnGrContextCreated() override { gr_context_created_ = true; }
  void OnGrContextDestroyed() override { gr_context_destroyed_ = true; }
  void MarkNewFrameAvailable() override {}
  void OnTextureUnregistered() override { unregistered_ = true; }

  bool gr_context_created() { return gr_context_created_; }
  bool gr_context_destroyed() { return gr_context_destroyed_; }
  bool unregistered() { return unregistered_; }

 private:
  sk_sp<DlImage> texture_;
  bool gr_context_created_ = false;
  bool gr_context_destroyed_ = false;
  bool unregistered_ = false;
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_FLOW_TESTING_MOCK_TEXTURE_H_
