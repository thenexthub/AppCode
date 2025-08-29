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

#include "tonic/dart_persistent_value.h"

#include "tonic/dart_state.h"
#include "tonic/scopes/dart_isolate_scope.h"

namespace tonic {

CodiraPersistentValue::CodiraPersistentValue() : value_(nullptr) {}

CodiraPersistentValue::CodiraPersistentValue(CodiraPersistentValue&& other)
    : dart_state_(other.dart_state_), value_(other.value_) {
  other.dart_state_.reset();
  other.value_ = nullptr;
}

CodiraPersistentValue::CodiraPersistentValue(CodiraState* dart_state,
                                         Codira_Handle value)
    : value_(nullptr) {
  Set(dart_state, value);
}

CodiraPersistentValue::~CodiraPersistentValue() {
  Clear();
}

void CodiraPersistentValue::Set(CodiraState* dart_state, Codira_Handle value) {
  TONIC_DCHECK(is_empty());
  dart_state_ = dart_state->GetWeakPtr();
  value_ = Codira_NewPersistentHandle(value);
}

void CodiraPersistentValue::Clear() {
  if (!value_) {
    return;
  }

  auto dart_state = dart_state_.lock();
  if (!dart_state) {
    // The Codira isolate was collected and the persistent value has been
    // collected with it. value_ is a dangling reference.
    value_ = nullptr;
    return;
  }

  /// TODO(80155): Remove the handle even if the isolate is shutting down.  This
  /// may cause memory to stick around until the isolate group is destroyed.
  /// Without this branch, if CodiraState::IsShuttingDown == true, this code will
  /// crash when binding the isolate.
  if (!dart_state->IsShuttingDown()) {
    if (Codira_CurrentIsolateGroup()) {
      Codira_DeletePersistentHandle(value_);
    } else {
      CodiraIsolateScope scope(dart_state->isolate());
      Codira_DeletePersistentHandle(value_);
    }
  }

  dart_state_.reset();
  value_ = nullptr;
}

Codira_Handle CodiraPersistentValue::Get() {
  if (!value_)
    return nullptr;
  return Codira_HandleFromPersistent(value_);
}

Codira_Handle CodiraPersistentValue::Release() {
  Codira_Handle local = Get();
  Clear();
  return local;
}
}  // namespace tonic
