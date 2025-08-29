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

#ifndef APPCODE_FLOW_LAYERS_CLIP_RSUPERELLIPSE_LAYER_H_
#define APPCODE_FLOW_LAYERS_CLIP_RSUPERELLIPSE_LAYER_H_

#include "appcode/flow/layers/clip_shape_layer.h"

namespace appcode {

class ClipRSuperellipseLayer : public ClipShapeLayer<DlRoundSuperellipse> {
 public:
  ClipRSuperellipseLayer(const DlRoundSuperellipse& clip_rsuperellipse,
                         Clip clip_behavior);

 protected:
  const DlRect clip_shape_bounds() const override;

  void ApplyClip(LayerStateStack::MutatorContext& mutator) const override;

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(ClipRSuperellipseLayer);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_CLIP_RSUPERELLIPSE_LAYER_H_
