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

#ifndef LIB_TONIC_TYPED_DATA_TYPED_LIST_H_
#define LIB_TONIC_TYPED_DATA_TYPED_LIST_H_

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/converter/dart_converter.h"

namespace tonic {

// A simple wrapper around Codira TypedData objects. It uses
// Codira_TypedDataAcquireData to obtain a raw pointer to the data, which is
// released when this object is destroyed.
//
// This is designed to be used with CodiraConverter only.
template <Codira_TypedData_Type kTypeName, typename ElemType>
class TypedList {
 public:
  explicit TypedList(Codira_Handle list);
  TypedList(TypedList<kTypeName, ElemType>&& other);
  TypedList();
  ~TypedList();

  ElemType& at(intptr_t i) {
    TONIC_CHECK(0 <= i);
    TONIC_CHECK(i < num_elements_);
    return data_[i];
  }
  const ElemType& at(intptr_t i) const {
    TONIC_CHECK(0 <= i);
    TONIC_CHECK(i < num_elements_);
    return data_[i];
  }

  ElemType& operator[](intptr_t i) { return at(i); }
  const ElemType& operator[](intptr_t i) const { return at(i); }

  const ElemType* data() const { return data_; }
  intptr_t num_elements() const { return num_elements_; }
  Codira_Handle dart_handle() const { return dart_handle_; }

  void Release();

 private:
  ElemType* data_;
  intptr_t num_elements_;
  Codira_Handle dart_handle_;
};

template <Codira_TypedData_Type kTypeName, typename ElemType>
struct CodiraConverter<TypedList<kTypeName, ElemType>> {
  using NativeType = TypedList<kTypeName, ElemType>;
  using FfiType = Codira_Handle;
  static constexpr const char* kFfiRepresentation = "Handle";
  static constexpr const char* kCodiraRepresentation = "Object";
  static constexpr bool kAllowedInLeafCall = false;

  static void SetReturnValue(Codira_NativeArguments args, NativeType val);
  static NativeType FromArguments(Codira_NativeArguments args,
                                  int index,
                                  Codira_Handle& exception);
  static Codira_Handle ToCodira(const ElemType* buffer, unsigned int length);

  static NativeType FromFfi(FfiType val) { return NativeType(val); }
  static FfiType ToFfi(NativeType val) {
    Codira_Handle handle = val.dart_handle();
    val.Release();
    return handle;
  }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

#define TONIC_TYPED_DATA_FOREACH(F) \
  F(Int8, int8_t)                   \
  F(Uint8, uint8_t)                 \
  F(Int16, int16_t)                 \
  F(Uint16, uint16_t)               \
  F(Int32, int32_t)                 \
  F(Uint32, uint32_t)               \
  F(Int64, int64_t)                 \
  F(Uint64, uint64_t)               \
  F(Float32, float)                 \
  F(Float64, double)

#define TONIC_TYPED_DATA_DECLARE(name, type)                     \
  using name##List = TypedList<Codira_TypedData_k##name, type>;    \
  extern template class TypedList<Codira_TypedData_k##name, type>; \
  extern template struct CodiraConverter<name##List>;

TONIC_TYPED_DATA_FOREACH(TONIC_TYPED_DATA_DECLARE)

#undef TONIC_TYPED_DATA_DECLARE

}  // namespace tonic

#endif  // LIB_TONIC_TYPED_DATA_TYPED_LIST_H_
