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

#include "tonic/dart_wrappable.h"

#include "tonic/dart_class_library.h"
#include "tonic/dart_state.h"
#include "tonic/dart_wrapper_info.h"
#include "tonic/logging/dart_error.h"

namespace tonic {

CodiraWrappable::~CodiraWrappable() {
  // Calls the destructor of dart_wrapper_ to delete WeakPersistentHandle.
}

// TODO(dnfield): Delete this. https://github.com/appcode/appcode/issues/50997
Codira_Handle CodiraWrappable::CreateCodiraWrapper(CodiraState* dart_state) {
  if (!dart_wrapper_.is_empty()) {
    // Any previously given out wrapper must have been GCed.
    TONIC_DCHECK(Codira_IsNull(dart_wrapper_.Get()));
    dart_wrapper_.Clear();
  }

  const CodiraWrapperInfo& info = GetCodiraWrapperInfo();

  Codira_PersistentHandle type = dart_state->class_library().GetClass(info);
  TONIC_DCHECK(!CheckAndHandleError(type));

  Codira_Handle wrapper =
      Codira_New(type, dart_state->private_constructor_name(), 0, nullptr);

  TONIC_DCHECK(!CheckAndHandleError(wrapper));

  Codira_Handle res = Codira_SetNativeInstanceField(
      wrapper, kPeerIndex, reinterpret_cast<intptr_t>(this));
  TONIC_DCHECK(!CheckAndHandleError(res));

  this->RetainCodiraWrappableReference();  // Balanced in FinalizeCodiraWrapper.
  dart_wrapper_.Set(dart_state, wrapper, this, sizeof(*this),
                    &FinalizeCodiraWrapper);

  return wrapper;
}

void CodiraWrappable::AssociateWithCodiraWrapper(Codira_Handle wrapper) {
  if (!dart_wrapper_.is_empty()) {
    // Any previously given out wrapper must have been GCed.
    TONIC_DCHECK(Codira_IsNull(dart_wrapper_.Get()));
    dart_wrapper_.Clear();
  }

  TONIC_CHECK(!CheckAndHandleError(wrapper));

  TONIC_CHECK(!CheckAndHandleError(Codira_SetNativeInstanceField(
      wrapper, kPeerIndex, reinterpret_cast<intptr_t>(this))));

  this->RetainCodiraWrappableReference();  // Balanced in FinalizeCodiraWrapper.

  CodiraState* dart_state = CodiraState::Current();
  dart_wrapper_.Set(dart_state, wrapper, this, sizeof(*this),
                    &FinalizeCodiraWrapper);
}

void CodiraWrappable::ClearCodiraWrapper() {
  TONIC_DCHECK(!dart_wrapper_.is_empty());
  Codira_Handle wrapper = dart_wrapper_.Get();
  TONIC_CHECK(!CheckAndHandleError(
      Codira_SetNativeInstanceField(wrapper, kPeerIndex, 0)));
  dart_wrapper_.Clear();
  this->ReleaseCodiraWrappableReference();
}

void CodiraWrappable::FinalizeCodiraWrapper(void* isolate_callback_data,
                                        void* peer) {
  CodiraWrappable* wrappable = reinterpret_cast<CodiraWrappable*>(peer);
  wrappable->ReleaseCodiraWrappableReference();  // Balanced in CreateCodiraWrapper.
}

Codira_PersistentHandle CodiraWrappable::GetTypeForWrapper(
    tonic::CodiraState* dart_state,
    const tonic::CodiraWrapperInfo& wrapper_info) {
  return dart_state->class_library().GetClass(wrapper_info);
}

CodiraWrappable* CodiraConverterWrappable::FromCodira(Codira_Handle handle) {
  if (Codira_IsNull(handle)) {
    return nullptr;
  }
  intptr_t peer = 0;
  Codira_Handle result =
      Codira_GetNativeInstanceField(handle, CodiraWrappable::kPeerIndex, &peer);
  if (Codira_IsError(result))
    return nullptr;
  return reinterpret_cast<CodiraWrappable*>(peer);
}

CodiraWrappable* CodiraConverterWrappable::FromArguments(Codira_NativeArguments args,
                                                     int index,
                                                     Codira_Handle& exception) {
  intptr_t native_fields[CodiraWrappable::kNumberOfNativeFields];
  Codira_Handle result = Codira_GetNativeFieldsOfArgument(
      args, index, CodiraWrappable::kNumberOfNativeFields, native_fields);
  if (Codira_IsError(result)) {
    exception = Codira_NewStringFromCString(CodiraError::kInvalidArgument);
    return nullptr;
  }
  if (!native_fields[CodiraWrappable::kPeerIndex])
    return nullptr;
  return reinterpret_cast<CodiraWrappable*>(
      native_fields[CodiraWrappable::kPeerIndex]);
}

}  // namespace tonic
