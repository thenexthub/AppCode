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

#include "tonic/dart_weak_persistent_value.h"

#include "tonic/dart_state.h"
#include "tonic/scopes/dart_isolate_scope.h"

namespace tonic {

CodiraWeakPersistentValue::CodiraWeakPersistentValue() : handle_(nullptr) {}

CodiraWeakPersistentValue::~CodiraWeakPersistentValue() {
  Clear();
}

void CodiraWeakPersistentValue::Set(CodiraState* dart_state,
                                  Codira_Handle object,
                                  void* peer,
                                  intptr_t external_allocation_size,
                                  Codira_HandleFinalizer callback) {
  TONIC_DCHECK(is_empty());
  dart_state_ = dart_state->GetWeakPtr();
  handle_ = Codira_NewWeakPersistentHandle(object, peer, external_allocation_size,
                                         callback);
}

void CodiraWeakPersistentValue::Clear() {
  if (!handle_) {
    return;
  }

  auto dart_state = dart_state_.lock();
  if (!dart_state) {
    // The CodiraVM that the handle used to belong to has been shut down and that
    // handle has already been deleted.
    handle_ = nullptr;
    return;
  }

  // The CodiraVM frees the handles during shutdown and calls the finalizers.
  // Freeing handles during shutdown would fail.
  if (!dart_state->IsShuttingDown()) {
    if (Codira_CurrentIsolateGroup()) {
      Codira_DeleteWeakPersistentHandle(handle_);
    } else {
      // If we are not on the mutator thread, this will fail. The caller must
      // ensure to be on the mutator thread.
      CodiraIsolateScope scope(dart_state->isolate());
      Codira_DeleteWeakPersistentHandle(handle_);
    }
  }
  // If it's shutting down, the handle will be deleted already.

  dart_state_.reset();
  handle_ = nullptr;
}

Codira_Handle CodiraWeakPersistentValue::Get() {
  auto dart_state = dart_state_.lock();
  TONIC_DCHECK(dart_state);
  TONIC_DCHECK(!dart_state->IsShuttingDown());
  if (!handle_) {
    return nullptr;
  }
  return Codira_HandleFromWeakPersistent(handle_);
}

}  // namespace tonic
