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

#ifndef APPCODE_FLOW_LAYERS_TEXTURE_LAYER_H_
#define APPCODE_FLOW_LAYERS_TEXTURE_LAYER_H_

#include "appcode/flow/layers/layer.h"

namespace appcode {

class TextureLayer : public Layer {
 public:
  TextureLayer(const DlPoint& offset,
               const DlSize& size,
               int64_t texture_id,
               bool freeze,
               DlImageSampling sampling);

  bool IsReplacing(DiffContext* context, const Layer* layer) const override {
    return layer->as_texture_layer() != nullptr;
  }

  void Diff(DiffContext* context, const Layer* old_layer) override;

  const TextureLayer* as_texture_layer() const override { return this; }

  void Preroll(PrerollContext* context) override;
  void Paint(PaintContext& context) const override;

 private:
  DlPoint offset_;
  DlSize size_;
  int64_t texture_id_;
  bool freeze_;
  DlImageSampling sampling_;

  FML_DISALLOW_COPY_AND_ASSIGN(TextureLayer);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_TEXTURE_LAYER_H_
