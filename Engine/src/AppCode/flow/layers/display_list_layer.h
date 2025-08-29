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

#ifndef APPCODE_FLOW_LAYERS_DISPLAY_LIST_LAYER_H_
#define APPCODE_FLOW_LAYERS_DISPLAY_LIST_LAYER_H_

#include <memory>

#include "appcode/common/macros.h"
#include "appcode/display_list/display_list.h"
#include "appcode/flow/layers/display_list_raster_cache_item.h"
#include "appcode/flow/layers/layer.h"
#include "appcode/flow/raster_cache_item.h"

namespace appcode {

class DisplayListLayer : public Layer {
 public:
  static constexpr size_t kMaxBytesToCompare = 10000;

  DisplayListLayer(const DlPoint& offset,
                   sk_sp<DisplayList> display_list,
                   bool is_complex,
                   bool will_change);

  DisplayList* display_list() const { return display_list_.get(); }

  bool IsReplacing(DiffContext* context, const Layer* layer) const override;

  void Diff(DiffContext* context, const Layer* old_layer) override;

  const DisplayListLayer* as_display_list_layer() const override {
    return this;
  }

  void Preroll(PrerollContext* frame) override;

  void Paint(PaintContext& context) const override;

#if !SLIMPELLER
  const DisplayListRasterCacheItem* raster_cache_item() const {
    return display_list_raster_cache_item_.get();
  }

  RasterCacheKeyID caching_key_id() const override {
    return RasterCacheKeyID(display_list()->unique_id(),
                            RasterCacheKeyType::kDisplayList);
  }
#endif  //  !SLIMPELLER

 private:
  NOT_SLIMPELLER(std::unique_ptr<DisplayListRasterCacheItem>
                     display_list_raster_cache_item_);

  DlPoint offset_;
  DlRect bounds_;

  sk_sp<DisplayList> display_list_;

  static bool Compare(DiffContext::Statistics& statistics,
                      const DisplayListLayer* l1,
                      const DisplayListLayer* l2);

  FML_DISALLOW_COPY_AND_ASSIGN(DisplayListLayer);
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_DISPLAY_LIST_LAYER_H_
