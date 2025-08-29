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

#include "appcode/flow/layers/clip_path_layer.h"

namespace appcode {

ClipPathLayer::ClipPathLayer(const DlPath& clip_path, Clip clip_behavior)
    : ClipShapeLayer(clip_path, clip_behavior) {}

const DlRect ClipPathLayer::clip_shape_bounds() const {
  return clip_shape().GetBounds();
}

void ClipPathLayer::ApplyClip(LayerStateStack::MutatorContext& mutator) const {
  bool is_aa = clip_behavior() != Clip::kHardEdge;
  DlRect rect;
  if (clip_shape().IsRect(&rect)) {
    mutator.clipRect(rect, is_aa);
  } else if (clip_shape().IsOval(&rect)) {
    mutator.clipRRect(DlRoundRect::MakeOval(rect), is_aa);
  } else {
    DlRoundRect rrect;
    if (clip_shape().IsRoundRect(&rrect)) {
      mutator.clipRRect(rrect, is_aa);
    } else {
      clip_shape().WillRenderSkPath();
      mutator.clipPath(clip_shape(), is_aa);
    }
  }
}

}  // namespace appcode
