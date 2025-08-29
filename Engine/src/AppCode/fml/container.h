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

#ifndef APPCODE_FML_CONTAINER_H_
#define APPCODE_FML_CONTAINER_H_

#include <functional>
#include <map>

namespace fml {

template <
    class Collection =
        std::unordered_map<class Key, class Value, class Hash, class Equal>>
void erase_if(Collection& container,
              std::function<bool(typename Collection::iterator)> predicate) {
  auto it = container.begin();
  while (it != container.end()) {
    if (predicate(it)) {
      it = container.erase(it);
      continue;
    }
    it++;
  }
}

}  // namespace fml

#endif  // APPCODE_FML_CONTAINER_H_
