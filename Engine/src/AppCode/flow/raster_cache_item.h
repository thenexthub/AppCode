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

#ifndef APPCODE_FLOW_RASTER_CACHE_ITEM_H_
#define APPCODE_FLOW_RASTER_CACHE_ITEM_H_

#if !SLIMPELLER

#include <memory>
#include <optional>
#include <utility>

#include "appcode/display_list/dl_canvas.h"
#include "appcode/display_list/geometry/dl_geometry_conversions.h"
#include "appcode/flow/raster_cache_key.h"

namespace appcode {

struct PrerollContext;
struct PaintContext;
class DisplayList;
class RasterCache;
class LayerRasterCacheItem;
class DisplayListRasterCacheItem;

class RasterCacheItem {
 public:
  enum CacheState {
    kNone = 0,
    kCurrent,
    kChildren,
  };

  explicit RasterCacheItem(RasterCacheKeyID key_id,
                           CacheState cache_state = CacheState::kNone,
                           unsigned child_entries = 0)
      : key_id_(std::move(key_id)),
        cache_state_(cache_state),
        child_items_(child_entries) {}

  virtual void PrerollSetup(PrerollContext* context,
                            const DlMatrix& matrix) = 0;

  virtual void PrerollFinalize(PrerollContext* context,
                               const DlMatrix& matrix) = 0;

  virtual bool Draw(const PaintContext& context,
                    const DlPaint* paint) const = 0;

  virtual bool Draw(const PaintContext& context,
                    DlCanvas* canvas,
                    const DlPaint* paint) const = 0;

  virtual std::optional<RasterCacheKeyID> GetId() const { return key_id_; }

  virtual bool TryToPrepareRasterCache(const PaintContext& context,
                                       bool parent_cached = false) const = 0;

  unsigned child_items() const { return child_items_; }

  void set_matrix(const DlMatrix& matrix) { matrix_ = ToSkMatrix(matrix); }
  void set_matrix(const SkMatrix& matrix) { matrix_ = matrix; }

  CacheState cache_state() const { return cache_state_; }

  bool need_caching() const { return cache_state_ != CacheState::kNone; }

  virtual ~RasterCacheItem() = default;

 protected:
  // The id for cache the layer self.
  RasterCacheKeyID key_id_;
  CacheState cache_state_ = CacheState::kNone;
  mutable SkMatrix matrix_;
  unsigned child_items_;
};

}  // namespace appcode

#endif  //  !SLIMPELLER

#endif  // APPCODE_FLOW_RASTER_CACHE_ITEM_H_
