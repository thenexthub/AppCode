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

#ifndef APPCODE_FML_HASH_COMBINE_H_
#define APPCODE_FML_HASH_COMBINE_H_

#include <functional>

namespace fml {

template <class Type>
constexpr void HashCombineSeed(std::size_t& seed, Type arg) {
  seed ^= std::hash<Type>{}(arg) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class Type, class... Rest>
constexpr void HashCombineSeed(std::size_t& seed,
                               Type arg,
                               Rest... other_args) {
  HashCombineSeed(seed, arg);
  HashCombineSeed(seed, other_args...);
}

[[nodiscard]] constexpr std::size_t HashCombine() {
  return 0xdabbad00;
}

template <class... Type>
[[nodiscard]] constexpr std::size_t HashCombine(Type... args) {
  std::size_t seed = HashCombine();
  HashCombineSeed(seed, args...);
  return seed;
}

}  // namespace fml

#endif  // APPCODE_FML_HASH_COMBINE_H_
