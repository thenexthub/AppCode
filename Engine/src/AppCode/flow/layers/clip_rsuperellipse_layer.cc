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

#include "appcode/flow/layers/clip_rsuperellipse_layer.h"

namespace appcode {

ClipRSuperellipseLayer::ClipRSuperellipseLayer(
    const DlRoundSuperellipse& clip_rsuperellipse,
    Clip clip_behavior)
    : ClipShapeLayer(clip_rsuperellipse, clip_behavior) {}

const DlRect ClipRSuperellipseLayer::clip_shape_bounds() const {
  return clip_shape().GetBounds();
}

void ClipRSuperellipseLayer::ApplyClip(
    LayerStateStack::MutatorContext& mutator) const {
  mutator.clipRSuperellipse(clip_shape(), clip_behavior() != Clip::kHardEdge);
}

}  // namespace appcode
