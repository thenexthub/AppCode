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

#ifndef APPCODE_FLOW_LAYERS_SHADER_MASK_LAYER_H_
#define APPCODE_FLOW_LAYERS_SHADER_MASK_LAYER_H_

#include "appcode/display_list/effects/dl_color_source.h"
#include "appcode/flow/layers/cacheable_layer.h"

namespace appcode {

class ShaderMaskLayer : public CacheableContainerLayer {
 public:
  ShaderMaskLayer(std::shared_ptr<DlColorSource> color_source,
                  const DlRect& mask_rect,
                  DlBlendMode blend_mode);

  void Diff(DiffContext* context, const Layer* old_layer) override;

  void Preroll(PrerollContext* context) override;

  void Paint(PaintContext& context) const override;

 private:
  std::shared_ptr<DlColorSource> color_source_;
  DlRect mask_rect_;
  DlBlendMode blend_mode_;

  FML_DISALLOW_COPY_AND_ASSIGN(ShaderMaskLayer);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_SHADER_MASK_LAYER_H_
