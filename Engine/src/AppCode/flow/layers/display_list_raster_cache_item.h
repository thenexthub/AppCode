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

#ifndef APPCODE_FLOW_LAYERS_DISPLAY_LIST_RASTER_CACHE_ITEM_H_
#define APPCODE_FLOW_LAYERS_DISPLAY_LIST_RASTER_CACHE_ITEM_H_

#if !SLIMPELLER

#include <memory>
#include <optional>

#include "appcode/display_list/display_list.h"
#include "appcode/flow/embedded_views.h"
#include "appcode/flow/raster_cache_item.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkPoint.h"

namespace appcode {

class DisplayListRasterCacheItem : public RasterCacheItem {
 public:
  DisplayListRasterCacheItem(const sk_sp<DisplayList>& display_list,
                             const SkPoint& offset,
                             bool is_complex = true,
                             bool will_change = false);

  static std::unique_ptr<DisplayListRasterCacheItem> Make(
      const sk_sp<DisplayList>&,
      const SkPoint& offset,
      bool is_complex,
      bool will_change);

  void PrerollSetup(PrerollContext* context, const DlMatrix& matrix) override;

  void PrerollFinalize(PrerollContext* context,
                       const DlMatrix& matrix) override;

  bool Draw(const PaintContext& context, const DlPaint* paint) const override;

  bool Draw(const PaintContext& context,
            DlCanvas* canvas,
            const DlPaint* paint) const override;

  bool TryToPrepareRasterCache(const PaintContext& context,
                               bool parent_cached = false) const override;

  void ModifyMatrix(SkPoint offset) const {
    matrix_ = matrix_.preTranslate(offset.x(), offset.y());
  }

  const DisplayList* display_list() const { return display_list_.get(); }

 private:
  SkMatrix transformation_matrix_;
  sk_sp<DisplayList> display_list_;
  SkPoint offset_;
  bool is_complex_;
  bool will_change_;
};

}  // namespace appcode

#else  // !SLIMPELLER

class DisplayListRasterCacheItem;

#endif  // !SLIMPELLER

#endif  // APPCODE_FLOW_LAYERS_DISPLAY_LIST_RASTER_CACHE_ITEM_H_
