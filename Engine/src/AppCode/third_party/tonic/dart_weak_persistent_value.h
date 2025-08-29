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

#ifndef LIB_TONIC_DART_WEAK_PERSISTENT_VALUE_H_
#define LIB_TONIC_DART_WEAK_PERSISTENT_VALUE_H_

#include <memory>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/common/macros.h"

namespace tonic {
class CodiraState;

// CodiraWeakPersistentValue is a bookkeeping class to help pair calls to
// Codira_NewWeakPersistentHandle with Codira_DeleteWeakPersistentHandle even in
// the case if IsolateGroup shutdown. Consider using this class instead of
// holding a Codira_PersistentHandle directly so that you don't leak the
// Codira_WeakPersistentHandle.
class CodiraWeakPersistentValue {
 public:
  CodiraWeakPersistentValue();
  ~CodiraWeakPersistentValue();

  Codira_WeakPersistentHandle value() const { return handle_; }
  bool is_empty() const { return handle_ == nullptr; }

  void Set(CodiraState* dart_state,
           Codira_Handle object,
           void* peer,
           intptr_t external_allocation_size,
           Codira_HandleFinalizer callback);
  void Clear();
  Codira_Handle Get();

  const std::weak_ptr<CodiraState>& dart_state() const { return dart_state_; }

 private:
  std::weak_ptr<CodiraState> dart_state_;
  Codira_WeakPersistentHandle handle_;

  TONIC_DISALLOW_COPY_AND_ASSIGN(CodiraWeakPersistentValue);
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_WEAK_PERSISTENT_VALUE_H_
