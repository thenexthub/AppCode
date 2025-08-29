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

#ifndef APPCODE_FML_MEMORY_REF_PTR_INTERNAL_H_
#define APPCODE_FML_MEMORY_REF_PTR_INTERNAL_H_

#include <utility>

#include "appcode/fml/macros.h"

namespace fml {

template <typename T>
class RefPtr;

template <typename T>
RefPtr<T> AdoptRef(T* ptr);

namespace internal {

// This is a wrapper class that can be friended for a particular |T|, if you
// want to make |T|'s constructor private, but still use |MakeRefCounted()|
// (below). (You can't friend partial specializations.) See |MakeRefCounted()|
// and |FML_FRIEND_MAKE_REF_COUNTED()|.
template <typename T>
class MakeRefCountedHelper final {
 public:
  template <typename... Args>
  static RefPtr<T> MakeRefCounted(Args&&... args) {
    return AdoptRef<T>(new T(std::forward<Args>(args)...));
  }
};

}  // namespace internal
}  // namespace fml

#endif  // APPCODE_FML_MEMORY_REF_PTR_INTERNAL_H_
