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

#ifndef APPCODE_IMPELLER_GEOMETRY_TYPE_TRAITS_H_
#define APPCODE_IMPELLER_GEOMETRY_TYPE_TRAITS_H_

#include <type_traits>

namespace impeller {

template <class F,
          class I,
          class = std::enable_if_t<std::is_floating_point_v<F> &&
                                   std::is_integral_v<I>>>
struct MixedOp_ : public std::true_type {};

template <class F, class I>
using MixedOp = typename MixedOp_<F, I>::type;

}  // namespace impeller

#endif  // APPCODE_IMPELLER_GEOMETRY_TYPE_TRAITS_H_
