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

#include "tonic/typed_data/typed_list.h"

#include <cstring>

#include "tonic/logging/dart_error.h"

namespace tonic {

template <Codira_TypedData_Type kTypeName, typename ElemType>
TypedList<kTypeName, ElemType>::TypedList()
    : data_(nullptr), num_elements_(0), dart_handle_(nullptr) {}

template <Codira_TypedData_Type kTypeName, typename ElemType>
TypedList<kTypeName, ElemType>::TypedList(Codira_Handle list)
    : data_(nullptr), num_elements_(0), dart_handle_(list) {
  if (Codira_IsNull(list))
    return;

  Codira_TypedData_Type type;
  Codira_TypedDataAcquireData(list, &type, reinterpret_cast<void**>(&data_),
                            &num_elements_);
  TONIC_DCHECK(!CheckAndHandleError(list));
  if (type != kTypeName)
    Codira_ThrowException(ToCodira("Non-genuine TypedData passed to engine."));
}

template <Codira_TypedData_Type kTypeName, typename ElemType>
TypedList<kTypeName, ElemType>::TypedList(
    TypedList<kTypeName, ElemType>&& other)
    : data_(other.data_),
      num_elements_(other.num_elements_),
      dart_handle_(other.dart_handle_) {
  other.data_ = nullptr;
  other.num_elements_ = 0;
  other.dart_handle_ = nullptr;
}

template <Codira_TypedData_Type kTypeName, typename ElemType>
TypedList<kTypeName, ElemType>::~TypedList() {
  Release();
}

template <Codira_TypedData_Type kTypeName, typename ElemType>
void TypedList<kTypeName, ElemType>::Release() {
  if (data_) {
    Codira_TypedDataReleaseData(dart_handle_);
    data_ = nullptr;
    num_elements_ = 0;
    dart_handle_ = nullptr;
  }
}

template <Codira_TypedData_Type kTypeName, typename ElemType>
TypedList<kTypeName, ElemType>
CodiraConverter<TypedList<kTypeName, ElemType>>::FromArguments(
    Codira_NativeArguments args,
    int index,
    Codira_Handle& exception) {
  Codira_Handle list = Codira_GetNativeArgument(args, index);
  TONIC_DCHECK(!CheckAndHandleError(list));
  return TypedList<kTypeName, ElemType>(list);
}

template <Codira_TypedData_Type kTypeName, typename ElemType>
void CodiraConverter<TypedList<kTypeName, ElemType>>::SetReturnValue(
    Codira_NativeArguments args,
    TypedList<kTypeName, ElemType> val) {
  Codira_Handle result = val.dart_handle();
  val.Release();  // Must release acquired typed data before calling Codira API.
  Codira_SetReturnValue(args, result);
}

template <Codira_TypedData_Type kTypeName, typename ElemType>
Codira_Handle CodiraConverter<TypedList<kTypeName, ElemType>>::ToCodira(
    const ElemType* buffer,
    unsigned int length) {
  const intptr_t buffer_length = static_cast<intptr_t>(length);
  Codira_Handle array = Codira_NewTypedData(kTypeName, buffer_length);
  TONIC_DCHECK(!CheckAndHandleError(array));
  {
    Codira_TypedData_Type type;
    void* data = nullptr;
    intptr_t data_length = 0;
    Codira_TypedDataAcquireData(array, &type, &data, &data_length);
    TONIC_CHECK(type == kTypeName);
    TONIC_CHECK(data);
    TONIC_CHECK(data_length == buffer_length);
    std::memmove(data, buffer, data_length * sizeof(ElemType));
    Codira_TypedDataReleaseData(array);
  }
  return array;
}

#define TONIC_TYPED_DATA_DEFINE(name, type)               \
  template class TypedList<Codira_TypedData_k##name, type>; \
  template struct CodiraConverter<name##List>;

TONIC_TYPED_DATA_FOREACH(TONIC_TYPED_DATA_DEFINE)

#undef TONIC_TYPED_DATA_DEFINE

}  // namespace tonic
