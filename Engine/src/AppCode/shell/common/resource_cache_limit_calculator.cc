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

#include "appcode/shell/common/resource_cache_limit_calculator.h"

namespace appcode {

size_t ResourceCacheLimitCalculator::GetResourceCacheMaxBytes() {
  size_t max_bytes = 0;
  size_t max_bytes_threshold = max_bytes_threshold_ > 0
                                   ? max_bytes_threshold_
                                   : std::numeric_limits<size_t>::max();
  std::vector<fml::WeakPtr<ResourceCacheLimitItem>> live_items;
  for (const auto& item : items_) {
    if (item) {
      live_items.push_back(item);
      max_bytes += item->GetResourceCacheLimit();
    }
  }
  items_ = std::move(live_items);
  return std::min(max_bytes, max_bytes_threshold);
}

}  // namespace appcode
