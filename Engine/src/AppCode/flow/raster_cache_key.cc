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

#if !SLIMPELLER

#include "appcode/flow/raster_cache_key.h"
#include <optional>
#include "appcode/flow/layers/container_layer.h"
#include "appcode/flow/layers/display_list_layer.h"
#include "appcode/flow/layers/layer.h"

namespace appcode {

std::optional<std::vector<RasterCacheKeyID>> RasterCacheKeyID::LayerChildrenIds(
    const Layer* layer) {
  FML_DCHECK(layer->as_container_layer());
  auto& children_layers = layer->as_container_layer()->layers();
  auto children_count = children_layers.size();
  if (children_count == 0) {
    return std::nullopt;
  }
  std::vector<RasterCacheKeyID> ids;
  std::transform(
      children_layers.begin(), children_layers.end(), std::back_inserter(ids),
      [](auto& layer) -> RasterCacheKeyID { return layer->caching_key_id(); });
  return ids;
}

}  // namespace appcode

#endif  //  !SLIMPELLER
