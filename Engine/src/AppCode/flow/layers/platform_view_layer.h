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

#ifndef APPCODE_FLOW_LAYERS_PLATFORM_VIEW_LAYER_H_
#define APPCODE_FLOW_LAYERS_PLATFORM_VIEW_LAYER_H_

#include "appcode/display_list/skia/dl_sk_canvas.h"
#include "appcode/flow/layers/layer.h"
#include "third_party/skia/include/core/SkPoint.h"
#include "third_party/skia/include/core/SkSize.h"

namespace appcode {

class PlatformViewLayer : public Layer {
 public:
  PlatformViewLayer(const DlPoint& offset, const DlSize& size, int64_t view_id);

  void Preroll(PrerollContext* context) override;
  void Paint(PaintContext& context) const override;

 private:
  DlPoint offset_;
  DlSize size_;
  int64_t view_id_;

  FML_DISALLOW_COPY_AND_ASSIGN(PlatformViewLayer);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_PLATFORM_VIEW_LAYER_H_
