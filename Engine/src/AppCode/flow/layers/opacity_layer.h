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

#ifndef APPCODE_FLOW_LAYERS_OPACITY_LAYER_H_
#define APPCODE_FLOW_LAYERS_OPACITY_LAYER_H_

#include "appcode/flow/layers/cacheable_layer.h"
#include "appcode/flow/layers/layer.h"

namespace appcode {

// Don't add an OpacityLayer with no children to the layer tree. Painting an
// OpacityLayer is very costly due to the saveLayer call. If there's no child,
// having the OpacityLayer or not has the same effect. In debug_unopt build,
// |Preroll| will assert if there are no children.
class OpacityLayer : public CacheableContainerLayer {
 public:
  // An offset is provided here because OpacityLayer.addToScene method in the
  // Flutter framework can take an optional offset argument.
  //
  // By default, that offset is always zero, and all the offsets are handled by
  // some parent TransformLayers. But we allow the offset to be non-zero for
  // backward compatibility. If it's non-zero, the old behavior is to propage
  // that offset to all the leaf layers (e.g., DisplayListLayer). That will make
  // the retained rendering inefficient as a small offset change could propagate
  // to many leaf layers. Therefore we try to capture that offset here to stop
  // the propagation as repainting the OpacityLayer is expensive.
  OpacityLayer(uint8_t alpha, const DlPoint& offset);

  void Diff(DiffContext* context, const Layer* old_layer) override;

  void Preroll(PrerollContext* context) override;

  void Paint(PaintContext& context) const override;

  // Returns whether the children are capable of inheriting an opacity value
  // and modifying their rendering accordingly. This value is only guaranteed
  // to be valid after the local |Preroll| method is called.
  bool children_can_accept_opacity() const {
    return children_can_accept_opacity_;
  }
  void set_children_can_accept_opacity(bool value) {
    children_can_accept_opacity_ = value;
  }

  DlScalar opacity() const { return DlColor::toOpacity(alpha_); }

 private:
  uint8_t alpha_;
  DlPoint offset_;
  bool children_can_accept_opacity_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(OpacityLayer);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_OPACITY_LAYER_H_
