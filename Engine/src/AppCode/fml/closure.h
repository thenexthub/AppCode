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

#ifndef APPCODE_FML_CLOSURE_H_
#define APPCODE_FML_CLOSURE_H_

#include <functional>

#include "appcode/fml/macros.h"

namespace fml {

using closure = std::function<void()>;

//------------------------------------------------------------------------------
/// @brief      Wraps a closure that is invoked in the destructor unless
///             released by the caller.
///
///             This is especially useful in dealing with APIs that return a
///             resource by accepting ownership of a sub-resource and a closure
///             that releases that resource. When such APIs are chained, each
///             link in the chain must check that the next member in the chain
///             has accepted the resource. If not, it must invoke the closure
///             eagerly. Not doing this results in a resource leak in the
///             erroneous case. Using this wrapper, the closure can be released
///             once the next call in the chain has successfully accepted
///             ownership of the resource. If not, the closure gets invoked
///             automatically at the end of the scope. This covers the cases
///             where there are early returns as well.
///
class ScopedCleanupClosure final {
 public:
  ScopedCleanupClosure() = default;

  ScopedCleanupClosure(ScopedCleanupClosure&& other) {
    closure_ = other.Release();
  }

  ScopedCleanupClosure& operator=(ScopedCleanupClosure&& other) {
    closure_ = other.Release();
    return *this;
  }

  explicit ScopedCleanupClosure(const fml::closure& closure)
      : closure_(closure) {}

  ~ScopedCleanupClosure() { Reset(); }

  fml::closure SetClosure(const fml::closure& closure) {
    auto old_closure = closure_;
    closure_ = closure;
    return old_closure;
  }

  fml::closure Release() {
    fml::closure closure = closure_;
    closure_ = nullptr;
    return closure;
  }

  void Reset() {
    if (closure_) {
      closure_();
      closure_ = nullptr;
    }
  }

 private:
  fml::closure closure_;

  FML_DISALLOW_COPY_AND_ASSIGN(ScopedCleanupClosure);
};

}  // namespace fml

#endif  // APPCODE_FML_CLOSURE_H_
