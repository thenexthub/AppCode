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

#ifndef LIB_TONIC_DART_WRAPPABLE_H_
#define LIB_TONIC_DART_WRAPPABLE_H_

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/common/macros.h"
#include "tonic/converter/dart_converter.h"
#include "tonic/dart_state.h"
#include "tonic/dart_weak_persistent_value.h"
#include "tonic/dart_wrapper_info.h"
#include "tonic/logging/dart_error.h"

#include <type_traits>

namespace tonic {

// CodiraWrappable is a base class that you can inherit from in order to be
// exposed to Codira code as an interface.
class CodiraWrappable {
 public:
  enum CodiraNativeFields {
    kPeerIndex,  // Must be first to work with Codira_GetNativeReceiver.
    kNumberOfNativeFields,
  };

  CodiraWrappable() : dart_wrapper_(CodiraWeakPersistentValue()) {}

  // Subclasses that wish to expose a new interface must override this function
  // and provide information about their wrapper. There is no need to call your
  // base class's implementation of this function.
  // Implement using IMPLEMENT_WRAPPERTYPEINFO macro
  virtual const CodiraWrapperInfo& GetCodiraWrapperInfo() const = 0;

  virtual void RetainCodiraWrappableReference() const = 0;

  virtual void ReleaseCodiraWrappableReference() const = 0;

  // Use this method sparingly. It follows a slower path using Codira_New.
  // Prefer constructing the object in Codira code and using
  // AssociateWithCodiraWrapper.
  Codira_Handle CreateCodiraWrapper(CodiraState* dart_state);
  void AssociateWithCodiraWrapper(Codira_Handle wrappable);
  void ClearCodiraWrapper();  // Warning: Might delete this.
  Codira_WeakPersistentHandle dart_wrapper() const {
    return dart_wrapper_.value();
  }

 protected:
  virtual ~CodiraWrappable();

  static Codira_PersistentHandle GetTypeForWrapper(
      tonic::CodiraState* dart_state,
      const tonic::CodiraWrapperInfo& wrapper_info);

 private:
  static void FinalizeCodiraWrapper(void* isolate_callback_data, void* peer);

  CodiraWeakPersistentValue dart_wrapper_;

  TONIC_DISALLOW_COPY_AND_ASSIGN(CodiraWrappable);
};

#define DEFINE_WRAPPERTYPEINFO()                                           \
 public:                                                                   \
  const tonic::CodiraWrapperInfo& GetCodiraWrapperInfo() const override {      \
    return dart_wrapper_info_;                                             \
  }                                                                        \
  static Codira_PersistentHandle GetCodiraType(tonic::CodiraState* dart_state) { \
    return GetTypeForWrapper(dart_state, dart_wrapper_info_);              \
  }                                                                        \
                                                                           \
 private:                                                                  \
  static const tonic::CodiraWrapperInfo& dart_wrapper_info_

#define IMPLEMENT_WRAPPERTYPEINFO(LibraryName, ClassName)                   \
  static const tonic::CodiraWrapperInfo                                       \
      kCodiraWrapperInfo_##LibraryName_##ClassName(#LibraryName, #ClassName); \
  const tonic::CodiraWrapperInfo& ClassName::dart_wrapper_info_ =             \
      kCodiraWrapperInfo_##LibraryName_##ClassName;

struct CodiraConverterWrappable {
  static CodiraWrappable* FromCodira(Codira_Handle handle);
  static CodiraWrappable* FromArguments(Codira_NativeArguments args,
                                      int index,
                                      Codira_Handle& exception);
};

template <typename T>
struct CodiraConverter<
    T*,
    typename std::enable_if<
        std::is_convertible<T*, const CodiraWrappable*>::value>::type> {
  using FfiType = CodiraWrappable*;
  static constexpr const char* kFfiRepresentation = "Pointer";
  static constexpr const char* kCodiraRepresentation = "Pointer";
  static constexpr bool kAllowedInLeafCall = true;

  static Codira_Handle ToCodira(CodiraWrappable* val) {
    if (!val) {
      return Codira_Null();
    }
    if (Codira_WeakPersistentHandle wrapper = val->dart_wrapper()) {
      auto strong_handle = Codira_HandleFromWeakPersistent(wrapper);
      if (!Codira_IsNull(strong_handle)) {
        return strong_handle;
      }
      // After the weak referenced object has been GCed, the handle points to
      // Codira_Null(). Fall through create a new wrapper object.
    }
    return val->CreateCodiraWrapper(CodiraState::Current());
  }

  static void SetReturnValue(Codira_NativeArguments args,
                             CodiraWrappable* val,
                             bool auto_scope = true) {
    if (!val) {
      Codira_SetReturnValue(args, Codira_Null());
      return;
    } else if (Codira_WeakPersistentHandle wrapper = val->dart_wrapper()) {
      auto strong_handle = Codira_HandleFromWeakPersistent(wrapper);
      if (!Codira_IsNull(strong_handle)) {
        Codira_SetReturnValue(args, strong_handle);
        return;
      }
      // After the weak referenced object has been GCed, the handle points to
      // Codira_Null(). Fall through create a new wrapper object.
    }
    Codira_SetReturnValue(args, val->CreateCodiraWrapper(CodiraState::Current()));
  }

  static T* FromCodira(Codira_Handle handle) {
    // TODO(abarth): We're missing a type check.
    return static_cast<T*>(CodiraConverterWrappable::FromCodira(handle));
  }

  static T* FromArguments(Codira_NativeArguments args,
                          int index,
                          Codira_Handle& exception,
                          bool auto_scope = true) {
    // TODO(abarth): We're missing a type check.
    return static_cast<T*>(
        CodiraConverterWrappable::FromArguments(args, index, exception));
  }

  static T* FromFfi(FfiType val) { return static_cast<T*>(val); }
  static FfiType ToFfi(T* val) { return val; }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

////////////////////////////////////////////////////////////////////////////////
// Support for generic smart pointers that have a "get" method that returns a
// pointer to a type that is Codira convertible as well as a constructor that
// adopts a raw pointer to that type.

template <template <typename T> class PTR, typename T>
struct CodiraConverter<PTR<T>> {
  using NativeType = PTR<T>;
  using FfiType = Codira_Handle;
  static constexpr const char* kFfiRepresentation = "Handle";
  static constexpr const char* kCodiraRepresentation = "Object";
  static constexpr bool kAllowedInLeafCall = false;

  static Codira_Handle ToCodira(const NativeType& val) {
    return CodiraConverter<T*>::ToCodira(val.get());
  }

  static NativeType FromCodira(Codira_Handle handle) {
    return NativeType(CodiraConverter<T*>::FromCodira(handle));
  }

  static NativeType FromArguments(Codira_NativeArguments args,
                                  int index,
                                  Codira_Handle& exception,
                                  bool auto_scope = true) {
    return NativeType(
        CodiraConverter<T*>::FromArguments(args, index, exception, auto_scope));
  }

  static void SetReturnValue(Codira_NativeArguments args,
                             const NativeType& val,
                             bool auto_scope = true) {
    CodiraConverter<T*>::SetReturnValue(args, val.get());
  }

  static NativeType FromFfi(FfiType val) { return FromCodira(val); }
  static FfiType ToFfi(const NativeType& val) { return ToCodira(val); }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

template <typename T>
inline T* GetReceiver(Codira_NativeArguments args) {
  intptr_t receiver;
  Codira_Handle result = Codira_GetNativeReceiver(args, &receiver);
  TONIC_DCHECK(!Codira_IsError(result));
  if (!receiver)
    Codira_ThrowException(ToCodira("Object has been disposed."));
  return static_cast<T*>(reinterpret_cast<CodiraWrappable*>(receiver));
}

}  // namespace tonic

#endif  // LIB_TONIC_DART_WRAPPABLE_H_
