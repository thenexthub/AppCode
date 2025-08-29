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

#include "appcode/flow/layers/cacheable_layer.h"

namespace appcode {

AutoCache::AutoCache(RasterCacheItem* raster_cache_item,
                     PrerollContext* context,
                     const DlMatrix& matrix)
    : raster_cache_item_(raster_cache_item),
      context_(context),
      matrix_(matrix) {
#if !SLIMPELLER
  if (IsCacheEnabled()) {
    raster_cache_item->PrerollSetup(context, matrix);
  }
#endif  //  !SLIMPELLER
}

bool AutoCache::IsCacheEnabled() {
#if SLIMPELLER
  return false;
#else   // SLIMPELLER
  return raster_cache_item_ && context_ && context_->raster_cache;
#endif  //  SLIMPELLER
}

AutoCache::~AutoCache() {
#if !SLIMPELLER
  if (IsCacheEnabled()) {
    raster_cache_item_->PrerollFinalize(context_, matrix_);
  }
#endif  //  !SLIMPELLER
}

CacheableContainerLayer::CacheableContainerLayer(int layer_cached_threshold,
                                                 bool can_cache_children) {
#if !SLIMPELLER
  layer_raster_cache_item_ = LayerRasterCacheItem::Make(
      this, layer_cached_threshold, can_cache_children);
#endif  //  !SLIMPELLER
}

}  // namespace appcode
