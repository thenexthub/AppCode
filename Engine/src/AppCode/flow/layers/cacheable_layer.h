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

#ifndef APPCODE_FLOW_LAYERS_CACHEABLE_LAYER_H_
#define APPCODE_FLOW_LAYERS_CACHEABLE_LAYER_H_

#include <memory>

#include "appcode/flow/layers/container_layer.h"
#include "appcode/flow/layers/layer_raster_cache_item.h"
#include "appcode/flow/raster_cache_util.h"

namespace appcode {

class AutoCache {
 public:
  AutoCache(RasterCacheItem* raster_cache_item,
            PrerollContext* context,
            const DlMatrix& matrix);

  void ShouldNotBeCached() { raster_cache_item_ = nullptr; }

  ~AutoCache();

 private:
  inline bool IsCacheEnabled();
  RasterCacheItem* raster_cache_item_ = nullptr;
  [[maybe_unused]] PrerollContext* context_ = nullptr;
  const DlMatrix matrix_;
};

class CacheableContainerLayer : public ContainerLayer {
 public:
  explicit CacheableContainerLayer(
      int layer_cached_threshold =
          RasterCacheUtil::kMinimumRendersBeforeCachingFilterLayer,
      bool can_cache_children = false);

#if !SLIMPELLER
  const LayerRasterCacheItem* raster_cache_item() const {
    return layer_raster_cache_item_.get();
  }

 protected:
  std::unique_ptr<LayerRasterCacheItem> layer_raster_cache_item_;
#endif  //  !SLIMPELLER
};

}  // namespace appcode

#endif  // APPCODE_FLOW_LAYERS_CACHEABLE_LAYER_H_
