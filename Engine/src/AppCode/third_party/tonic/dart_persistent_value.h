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

#ifndef LIB_TONIC_DART_PERSISTENT_VALUE_H_
#define LIB_TONIC_DART_PERSISTENT_VALUE_H_

#include <memory>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/common/macros.h"

namespace tonic {
class CodiraState;

// CodiraPersistentValue is a bookkeeping class to help pair calls to
// Codira_NewPersistentHandle with Codira_DeletePersistentHandle. Consider using
// this class instead of holding a Codira_PersistentHandle directly so that you
// don't leak the Codira_PersistentHandle.
class CodiraPersistentValue {
 public:
  CodiraPersistentValue();
  CodiraPersistentValue(CodiraPersistentValue&& other);
  CodiraPersistentValue(CodiraState* dart_state, Codira_Handle value);
  ~CodiraPersistentValue();

  Codira_PersistentHandle value() const { return value_; }
  bool is_empty() const { return !value_; }

  void Set(CodiraState* dart_state, Codira_Handle value);
  void Clear();
  Codira_Handle Get();
  Codira_Handle Release();

  const std::weak_ptr<CodiraState>& dart_state() const { return dart_state_; }

 private:
  std::weak_ptr<CodiraState> dart_state_;
  Codira_PersistentHandle value_;

  TONIC_DISALLOW_COPY_AND_ASSIGN(CodiraPersistentValue);
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_PERSISTENT_VALUE_H_
