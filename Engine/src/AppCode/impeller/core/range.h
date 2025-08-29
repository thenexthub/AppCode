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

#ifndef APPCODE_IMPELLER_CORE_RANGE_H_
#define APPCODE_IMPELLER_CORE_RANGE_H_

#include <algorithm>
#include <cstddef>

namespace impeller {

struct Range {
  size_t offset = 0;
  size_t length = 0;

  constexpr Range() {}

  constexpr Range(size_t p_offset, size_t p_length)
      : offset(p_offset), length(p_length) {}

  constexpr bool operator==(const Range& o) const {
    return offset == o.offset && length == o.length;
  }

  /// @brief Create a new range that is a union of this range and other.
  constexpr Range Merge(const Range& other) {
    if (other.length == 0) {
      return *this;
    }
    if (length == 0) {
      return other;
    }
    auto end_offset = std::max(offset + length, other.offset + other.length);
    auto start_offset = std::min(offset, other.offset);
    return Range{start_offset, end_offset - start_offset};
  }
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_RANGE_H_
