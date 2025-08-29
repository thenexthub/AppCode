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

#ifndef APPCODE_IMPELLER_BASE_BACKEND_CAST_H_
#define APPCODE_IMPELLER_BASE_BACKEND_CAST_H_

namespace impeller {

template <class Sub, class Base>
class BackendCast {
 public:
  static Sub& Cast(Base& base) { return reinterpret_cast<Sub&>(base); }

  static const Sub& Cast(const Base& base) {
    return reinterpret_cast<const Sub&>(base);
  }

  static Sub* Cast(Base* base) { return reinterpret_cast<Sub*>(base); }

  static const Sub* Cast(const Base* base) {
    return reinterpret_cast<const Sub*>(base);
  }
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_BASE_BACKEND_CAST_H_
