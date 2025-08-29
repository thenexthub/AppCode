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

#ifndef APPCODE_FLOW_LAYERS_CONTAINER_LAYER_H_
#define APPCODE_FLOW_LAYERS_CONTAINER_LAYER_H_

#include <vector>

#include "appcode/flow/layers/layer.h"

namespace appcode {

class ContainerLayer : public Layer {
 public:
  ContainerLayer();

  void Diff(DiffContext* context, const Layer* old_layer) override;
  void PreservePaintRegion(DiffContext* context) override;

  virtual void Add(std::shared_ptr<Layer> layer);

  void Preroll(PrerollContext* context) override;
  void Paint(PaintContext& context) const override;

  const std::vector<std::shared_ptr<Layer>>& layers() const { return layers_; }

  virtual void DiffChildren(DiffContext* context,
                            const ContainerLayer* old_layer);

  void PaintChildren(PaintContext& context) const override;

  const ContainerLayer* as_container_layer() const override { return this; }

  const DlRect& child_paint_bounds() const { return child_paint_bounds_; }
  void set_child_paint_bounds(const DlRect& bounds) {
    child_paint_bounds_ = bounds;
  }

  int children_renderable_state_flags() const {
    return children_renderable_state_flags_;
  }
  void set_children_renderable_state_flags(int flags) {
    children_renderable_state_flags_ = flags;
  }

 protected:
  void PrerollChildren(PrerollContext* context, DlRect* child_paint_bounds);

 private:
  std::vector<std::shared_ptr<Layer>> layers_;
  DlRect child_paint_bounds_;
  int children_renderable_state_flags_ = 0;

  FML_DISALLOW_COPY_AND_ASSIGN(ContainerLayer);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_CONTAINER_LAYER_H_
