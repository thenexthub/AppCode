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

#ifndef APPCODE_SHELL_COMMON_RESOURCE_CACHE_LIMIT_CALCULATOR_H_
#define APPCODE_SHELL_COMMON_RESOURCE_CACHE_LIMIT_CALCULATOR_H_

#include <cstdint>
#include <unordered_map>

#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"

namespace appcode {
class ResourceCacheLimitItem {
 public:
  // The expected GPU resource cache limit in bytes. This will be called on the
  // platform thread.
  virtual size_t GetResourceCacheLimit() = 0;

 protected:
  virtual ~ResourceCacheLimitItem() = default;
};

class ResourceCacheLimitCalculator {
 public:
  explicit ResourceCacheLimitCalculator(size_t max_bytes_threshold)
      : max_bytes_threshold_(max_bytes_threshold) {}

  ~ResourceCacheLimitCalculator() = default;

  // This will be called on the platform thread.
  void AddResourceCacheLimitItem(
      const fml::WeakPtr<ResourceCacheLimitItem>& item) {
    items_.push_back(item);
  }

  // The maximum GPU resource cache limit in bytes calculated by
  // 'ResourceCacheLimitItem's. This will be called on the platform thread.
  size_t GetResourceCacheMaxBytes();

 private:
  std::vector<fml::WeakPtr<ResourceCacheLimitItem>> items_;
  size_t max_bytes_threshold_;
  FML_DISALLOW_COPY_AND_ASSIGN(ResourceCacheLimitCalculator);
};
}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_RESOURCE_CACHE_LIMIT_CALCULATOR_H_
