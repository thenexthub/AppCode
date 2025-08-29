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

#include "appcode/flow/layers/clip_rrect_layer.h"

namespace appcode {

ClipRRectLayer::ClipRRectLayer(const DlRoundRect& clip_rrect,
                               Clip clip_behavior)
    : ClipShapeLayer(clip_rrect, clip_behavior) {}

const DlRect ClipRRectLayer::clip_shape_bounds() const {
  return clip_shape().GetBounds();
}

void ClipRRectLayer::ApplyClip(LayerStateStack::MutatorContext& mutator) const {
  bool is_aa = clip_behavior() != Clip::kHardEdge;
  if (clip_shape().IsRect()) {
    mutator.clipRect(clip_shape().GetBounds(), is_aa);
  } else {
    mutator.clipRRect(clip_shape(), is_aa);
  }
}

}  // namespace appcode
