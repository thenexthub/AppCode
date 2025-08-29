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

#ifndef BASE_CONTAINER_UTILS_H_
#define BASE_CONTAINER_UTILS_H_

#include <algorithm>
#include <set>
#include <vector>

namespace base {

template <class T, class Allocator, class Predicate>
size_t EraseIf(std::vector<T, Allocator>& container, Predicate pred) {
  auto it = std::remove_if(container.begin(), container.end(), pred);
  size_t removed = std::distance(it, container.end());
  container.erase(it, container.end());
  return removed;
}

template <typename Container, typename Value>
bool Contains(const Container& container, const Value& value) {
  return container.find(value) != container.end();
}

template <typename T>
bool Contains(const std::vector<T>& container, const T& value) {
  return std::find(container.begin(), container.end(), value) !=
         container.end();
}

}  // namespace base

#endif  // BASE_CONTAINER_UTILS_H_
