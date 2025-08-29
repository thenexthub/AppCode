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

#ifndef APPCODE_FLOW_LAYERS_IMAGE_FILTER_LAYER_H_
#define APPCODE_FLOW_LAYERS_IMAGE_FILTER_LAYER_H_

#include <memory>
#include "appcode/flow/layers/cacheable_layer.h"

namespace appcode {

class ImageFilterLayer : public CacheableContainerLayer {
 public:
  explicit ImageFilterLayer(const std::shared_ptr<DlImageFilter>& filter,
                            const DlPoint& offset = DlPoint());

  void Diff(DiffContext* context, const Layer* old_layer) override;

  void Preroll(PrerollContext* context) override;

  void Paint(PaintContext& context) const override;

 private:
  DlPoint offset_;
  const std::shared_ptr<DlImageFilter> filter_;
  std::shared_ptr<DlImageFilter> transformed_filter_;

  FML_DISALLOW_COPY_AND_ASSIGN(ImageFilterLayer);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_IMAGE_FILTER_LAYER_H_
