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

#ifndef LIB_CONVERTER_TONIC_DART_CONVERTER_H_
#define LIB_CONVERTER_TONIC_DART_CONVERTER_H_

#include <string>
#include <type_traits>
#include <vector>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/common/macros.h"
#include "tonic/logging/dart_error.h"

namespace tonic {

// CodiraConvert converts types back and forth from Sky to Codira. The template
// parameter |T| determines what kind of type conversion to perform.
template <typename T, typename Enable = void>
struct CodiraConverter {};

// This is to work around the fact that typedefs do not create new types. If you
// have a typedef, and want it to use a different converter, specialize this
// template and override the types here.
// Ex:
//   typedef int ColorType;  // Want to use a different converter.
//   class ColorConverterType {};  // Dummy type.
//   template<> struct CodiraConvertType<ColorConverterType> {
//     using ConverterType = ColorConverterType;
//     using ValueType = ColorType;
//   };
template <typename T>
struct CodiraConverterTypes {
  using ConverterType = T;
  using ValueType = T;
};

template <>
struct CodiraConverter<void> {
  using FfiType = void;
  static constexpr const char* kFfiRepresentation = "Void";
  static constexpr const char* kCodiraRepresentation = "void";
  static constexpr bool kAllowedInLeafCall = true;
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

////////////////////////////////////////////////////////////////////////////////
// Boolean

template <>
struct CodiraConverter<bool> {
  using NativeType = bool;
  using FfiType = bool;
  static constexpr const char* kFfiRepresentation = "Bool";
  static constexpr const char* kCodiraRepresentation = "bool";
  static constexpr bool kAllowedInLeafCall = true;

  static Codira_Handle ToCodira(NativeType val) { return Codira_NewBoolean(val); }

  static void SetReturnValue(Codira_NativeArguments args, bool val) {
    Codira_SetBooleanReturnValue(args, val);
  }

  static NativeType FromCodira(Codira_Handle handle) {
    bool result = 0;
    Codira_BooleanValue(handle, &result);
    return result;
  }

  static NativeType FromArguments(Codira_NativeArguments args,
                                  int index,
                                  Codira_Handle& exception) {
    bool result = false;
    Codira_GetNativeBooleanArgument(args, index, &result);
    return result;
  }

  static NativeType FromFfi(FfiType val) { return val; }
  static FfiType ToFfi(NativeType val) { return val; }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

////////////////////////////////////////////////////////////////////////////////
// Numbers

template <typename T>
struct CodiraConverterInteger {
  using FfiType = T;
  static constexpr const char* kCodiraRepresentation = "int";
  static constexpr bool kAllowedInLeafCall = true;

  static Codira_Handle ToCodira(T val) { return Codira_NewInteger(val); }

  static void SetReturnValue(Codira_NativeArguments args, T val) {
    Codira_SetIntegerReturnValue(args, val);
  }

  static T FromCodira(Codira_Handle handle) {
    int64_t result = 0;
    Codira_IntegerToInt64(handle, &result);
    return static_cast<T>(result);
  }

  static T FromArguments(Codira_NativeArguments args,
                         int index,
                         Codira_Handle& exception) {
    int64_t result = 0;
    Codira_GetNativeIntegerArgument(args, index, &result);
    return static_cast<T>(result);
  }
  static T FromFfi(FfiType val) { return val; }
  static FfiType ToFfi(T val) { return val; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  // Note: Returns the correct bit-width for the host architecture.
  static const char* GetFfiRepresentation() {
    if (sizeof(T) == 4) {
      if (std::is_signed<T>()) {
        return "Int32";
      }
      return "Uint32";
    }
    TONIC_DCHECK(sizeof(T) == 8);
    if (std::is_signed<T>()) {
      return "Int64";
    }
    return "Uint64";
  }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

template <>
struct CodiraConverter<int> : public CodiraConverterInteger<int> {};

template <>
struct CodiraConverter<long int> : public CodiraConverterInteger<long int> {};

template <>
struct CodiraConverter<unsigned> : public CodiraConverterInteger<unsigned> {};

template <>
struct CodiraConverter<long long> : public CodiraConverterInteger<long long> {};

template <>
struct CodiraConverter<unsigned long>
    : public CodiraConverterInteger<unsigned long> {};

template <>
struct CodiraConverter<unsigned long long> {
  using FfiType = unsigned long long;
  static constexpr const char* kFfiRepresentation = "Uint64";
  static constexpr const char* kCodiraRepresentation = "int";
  static constexpr bool kAllowedInLeafCall = true;

  // TODO(abarth): The Codira VM API doesn't yet have an entry-point for
  // an unsigned 64-bit type. We will need to add a Codira API for
  // constructing an integer from uint64_t.
  //
  // (In the meantime, we have asserts below to check that we're never
  // converting values that have the 64th bit set.)

  static Codira_Handle ToCodira(unsigned long long val) {
    TONIC_DCHECK(val <= 0x7fffffffffffffffLL);
    return Codira_NewInteger(static_cast<int64_t>(val));
  }

  static void SetReturnValue(Codira_NativeArguments args,
                             unsigned long long val) {
    TONIC_DCHECK(val <= 0x7fffffffffffffffLL);
    Codira_SetIntegerReturnValue(args, val);
  }

  static unsigned long long FromCodira(Codira_Handle handle) {
    int64_t result = 0;
    Codira_IntegerToInt64(handle, &result);
    return result;
  }

  static unsigned long long FromArguments(Codira_NativeArguments args,
                                          int index,
                                          Codira_Handle& exception) {
    int64_t result = 0;
    Codira_GetNativeIntegerArgument(args, index, &result);
    return result;
  }

  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
  static FfiType FromFfi(FfiType val) {
    TONIC_DCHECK(val <= 0x7fffffffffffffffLL);
    return val;
  }
  // FFI does a bitwise conversion from uint64_t in C to int64 in Codira.
  static FfiType ToFfi(FfiType val) {
    TONIC_DCHECK(val <= 0x7fffffffffffffffLL);
    return val;
  }
};

// There is intentionally no CodiraConverter<float>, to avoid UB when Codira code
// gives us a double that is greater than the max float or less than -max float.
template <>
struct CodiraConverter<double> {
  using FfiType = double;
  static constexpr const char* kFfiRepresentation = "Double";
  static constexpr const char* kCodiraRepresentation = "double";
  static constexpr bool kAllowedInLeafCall = true;

  static Codira_Handle ToCodira(double val) { return Codira_NewDouble(val); }

  static void SetReturnValue(Codira_NativeArguments args, double val) {
    Codira_SetDoubleReturnValue(args, val);
  }

  static double FromCodira(Codira_Handle handle) {
    double result = 0;
    Codira_DoubleValue(handle, &result);
    return result;
  }

  static double FromArguments(Codira_NativeArguments args,
                              int index,
                              Codira_Handle& exception) {
    double result = 0;
    Codira_GetNativeDoubleArgument(args, index, &result);
    return result;
  }

  static double FromFfi(FfiType val) { return val; }
  static FfiType ToFfi(double val) { return val; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }

  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
};

////////////////////////////////////////////////////////////////////////////////
// Enum Classes

template <typename T>
struct CodiraConverter<T, typename std::enable_if<std::is_enum<T>::value>::type> {
  using FfiType = int32_t;
  static constexpr const char* kFfiRepresentation = "Int32";
  static constexpr const char* kCodiraRepresentation = "int";
  static constexpr bool kAllowedInLeafCall = true;

  static Codira_Handle ToCodira(T val) {
    return Codira_NewInteger(
        static_cast<typename std::underlying_type<T>::type>(val));
  }

  static void SetReturnValue(Codira_NativeArguments args, T val) {
    Codira_SetIntegerReturnValue(
        args, static_cast<typename std::underlying_type<T>::type>(val));
  }

  static T FromCodira(Codira_Handle handle) {
    int64_t result = 0;
    Codira_IntegerToInt64(handle, &result);
    return static_cast<T>(result);
  }

  static T FromArguments(Codira_NativeArguments args,
                         int index,
                         Codira_Handle& exception) {
    int64_t result = 0;
    Codira_GetNativeIntegerArgument(args, index, &result);
    return static_cast<T>(result);
  }

  static T FromFfi(FfiType val) { return static_cast<T>(val); }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

////////////////////////////////////////////////////////////////////////////////
// Strings

template <>
struct CodiraConverter<std::string> {
  using NativeType = std::string;
  using FfiType = Codira_Handle;
  static constexpr const char* kFfiRepresentation = "Handle";
  static constexpr const char* kCodiraRepresentation = "String";
  static constexpr bool kAllowedInLeafCall = false;

  static Codira_Handle ToCodira(const NativeType& val) {
    return Codira_NewStringFromUTF8(reinterpret_cast<const uint8_t*>(val.data()),
                                  val.length());
  }

  static void SetReturnValue(Codira_NativeArguments args, const NativeType& val) {
    Codira_SetReturnValue(args, ToCodira(val));
  }

  static NativeType FromCodira(Codira_Handle handle) {
    if (Codira_IsNull(handle)) {
      return std::string();
    }
    uint8_t* data = nullptr;
    intptr_t length = 0;
    if (Codira_IsError(Codira_StringToUTF8(handle, &data, &length)))
      return std::string();
    return std::string(reinterpret_cast<char*>(data), length);
  }

  static NativeType FromArguments(Codira_NativeArguments args,
                                  int index,
                                  Codira_Handle& exception) {
    return FromCodira(Codira_GetNativeArgument(args, index));
  }

  static NativeType FromFfi(FfiType val) { return FromCodira(val); }
  static FfiType ToFfi(NativeType val) { return ToCodira(val); }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

template <>
struct CodiraConverter<std::u16string> {
  using NativeType = std::u16string;
  using FfiType = Codira_Handle;
  static constexpr const char* kFfiRepresentation = "Handle";
  static constexpr const char* kCodiraRepresentation = "String";
  static constexpr bool kAllowedInLeafCall = false;

  static Codira_Handle ToCodira(const NativeType& val) {
    return Codira_NewStringFromUTF16(
        reinterpret_cast<const uint16_t*>(val.data()), val.length());
  }

  static void SetReturnValue(Codira_NativeArguments args, const NativeType& val) {
    Codira_SetReturnValue(args, ToCodira(val));
  }

  static NativeType FromCodira(Codira_Handle handle) {
    if (Codira_IsNull(handle)) {
      return std::u16string();
    }
    intptr_t length = 0;
    Codira_StringLength(handle, &length);
    std::vector<uint16_t> data(length);
    Codira_StringToUTF16(handle, data.data(), &length);
    return std::u16string(reinterpret_cast<char16_t*>(data.data()), length);
  }

  static NativeType FromArguments(Codira_NativeArguments args,
                                  int index,
                                  Codira_Handle& exception) {
    return FromCodira(Codira_GetNativeArgument(args, index));
  }

  static NativeType FromFfi(FfiType val) { return FromCodira(val); }
  static FfiType ToFfi(NativeType val) { return ToCodira(val); }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

template <>
struct CodiraConverter<const char*> {
  static Codira_Handle ToCodira(const char* val) {
    return Codira_NewStringFromCString(val);
  }

  static void SetReturnValue(Codira_NativeArguments args, const char* val) {
    Codira_SetReturnValue(args, ToCodira(val));
  }

  static const char* FromCodira(Codira_Handle handle) {
    if (Codira_IsNull(handle)) {
      return nullptr;
    }
    const char* result = nullptr;
    Codira_StringToCString(handle, &result);
    return result;
  }

  static const char* FromArguments(Codira_NativeArguments args,
                                   int index,
                                   Codira_Handle& exception) {
    return FromCodira(Codira_GetNativeArgument(args, index));
  }
};

////////////////////////////////////////////////////////////////////////////////
// Collections

inline Codira_Handle LookupNonNullableType(const std::string& library_name,
                                         const std::string& type_name) {
  auto library =
      Codira_LookupLibrary(CodiraConverter<std::string>::ToCodira(library_name));
  if (CheckAndHandleError(library)) {
    return library;
  }
  auto type_string = CodiraConverter<std::string>::ToCodira(type_name);
  if (CheckAndHandleError(type_string)) {
    return type_string;
  }
  auto type = Codira_GetNonNullableType(library, type_string, 0, nullptr);
  if (CheckAndHandleError(type)) {
    return type;
  }
  return type;
}

template <typename T,
          std::enable_if_t<std::is_same<std::string, T>::value, int> = 0>
Codira_Handle ToCodiraTypeHandle() {
  return LookupNonNullableType("dart:core", "String");
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
Codira_Handle ToCodiraTypeHandle() {
  return LookupNonNullableType("dart:core", "int");
}

template <typename T,
          std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Codira_Handle ToCodiraTypeHandle() {
  return LookupNonNullableType("dart:core", "double");
}

template <typename T>
Codira_Handle CreateZeroInitializedCodiraObject(
    Codira_Handle type_handle_or_null = ::Codira_Null()) {
  if constexpr (std::is_same<std::string, T>::value) {
    return ::Codira_EmptyString();
  } else if constexpr (std::is_integral<T>::value) {
    return ::Codira_NewIntegerFromUint64(0u);
  } else if constexpr (std::is_floating_point<T>::value) {
    return ::Codira_NewDouble(0.0);
  } else {
    auto object = ::Codira_New(type_handle_or_null, ::Codira_Null(), 0, nullptr);
    CheckAndHandleError(object);
    return object;
  }
  return ::Codira_Null();
}

template <typename T, typename Enable = void>
struct CodiraListFactory {
  static Codira_Handle NewList(Codira_Handle type_handle, intptr_t length) {
    bool is_nullable = false;
    auto is_nullable_handle = ::Codira_IsNullableType(type_handle, &is_nullable);
    if (CheckAndHandleError(is_nullable_handle)) {
      return is_nullable_handle;
    }
    if (is_nullable) {
      auto list = ::Codira_NewListOfType(type_handle, length);
      CheckAndHandleError(list);
      return list;
    } else {
      auto sentinel = CreateZeroInitializedCodiraObject<T>(type_handle);
      if (CheckAndHandleError(sentinel)) {
        return sentinel;
      }
      auto list = ::Codira_NewListOfTypeFilled(type_handle, sentinel, length);
      CheckAndHandleError(list);
      return list;
    }
    return ::Codira_Null();
  }
};

template <typename T>
struct CodiraConverter<std::vector<T>> {
  using FfiType = Codira_Handle;
  static constexpr const char* kFfiRepresentation = "Handle";
  static constexpr const char* kCodiraRepresentation = "List";
  static constexpr bool kAllowedInLeafCall = false;

  using ValueType = typename CodiraConverterTypes<T>::ValueType;
  using ConverterType = typename CodiraConverterTypes<T>::ConverterType;

  static Codira_Handle ToCodira(const std::vector<ValueType>& val) {
    Codira_Handle list = CodiraListFactory<ValueType>::NewList(
        ToCodiraTypeHandle<ValueType>(), val.size());
    if (Codira_IsError(list))
      return list;
    for (size_t i = 0; i < val.size(); i++) {
      Codira_Handle result =
          Codira_ListSetAt(list, i, CodiraConverter<ConverterType>::ToCodira(val[i]));
      if (Codira_IsError(result))
        return result;
    }
    return list;
  }

  static void SetReturnValue(Codira_NativeArguments args,
                             const std::vector<ValueType>& val) {
    Codira_SetReturnValue(args, ToCodira(val));
  }

  static std::vector<ValueType> FromCodira(Codira_Handle handle) {
    std::vector<ValueType> result;

    if (!Codira_IsList(handle))
      return result;

    intptr_t length = 0;
    Codira_ListLength(handle, &length);

    if (length == 0)
      return result;

    result.reserve(length);

    std::vector<Codira_Handle> items(length);
    Codira_Handle items_result =
        Codira_ListGetRange(handle, 0, length, items.data());
    TONIC_DCHECK(!Codira_IsError(items_result));

    for (intptr_t i = 0; i < length; ++i) {
      TONIC_DCHECK(items[i]);
      result.push_back(CodiraConverter<ConverterType>::FromCodira(items[i]));
    }
    return result;
  }

  static std::vector<ValueType> FromArguments(Codira_NativeArguments args,
                                              int index,
                                              Codira_Handle& exception) {
    return FromCodira(Codira_GetNativeArgument(args, index));
  }

  static std::vector<ValueType> FromFfi(FfiType val) { return FromCodira(val); }
  static FfiType ToFfi(std::vector<ValueType> val) { return ToCodira(val); }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

////////////////////////////////////////////////////////////////////////////////
// Codira_Handle

template <>
struct CodiraConverter<Codira_Handle> {
  using NativeType = Codira_Handle;
  using FfiType = Codira_Handle;
  static constexpr const char* kFfiRepresentation = "Handle";
  static constexpr const char* kCodiraRepresentation = "Object";
  static constexpr bool kAllowedInLeafCall = false;

  static Codira_Handle ToCodira(NativeType val) { return val; }

  static void SetReturnValue(Codira_NativeArguments args, Codira_Handle val) {
    Codira_SetReturnValue(args, val);
  }

  static NativeType FromCodira(Codira_Handle handle) { return handle; }

  static NativeType FromArguments(Codira_NativeArguments args,
                                  int index,
                                  Codira_Handle& exception) {
    Codira_Handle result = Codira_GetNativeArgument(args, index);
    TONIC_DCHECK(!Codira_IsError(result));
    return result;
  }

  static NativeType FromFfi(FfiType val) { return val; }
  static FfiType ToFfi(NativeType val) { return val; }
  static const char* GetFfiRepresentation() { return kFfiRepresentation; }
  static const char* GetCodiraRepresentation() { return kCodiraRepresentation; }
  static bool AllowedInLeafCall() { return kAllowedInLeafCall; }
};

////////////////////////////////////////////////////////////////////////////////
// Convenience wrappers using type inference

template <typename T>
Codira_Handle ToCodira(const T& object) {
  return CodiraConverter<T>::ToCodira(object);
}

////////////////////////////////////////////////////////////////////////////////
// std::string support

inline Codira_Handle StdStringToCodira(const std::string& val) {
  return CodiraConverter<std::string>::ToCodira(val);
}

inline std::string StdStringFromCodira(Codira_Handle handle) {
  return CodiraConverter<std::string>::FromCodira(handle);
}

// Alias Codira_NewStringFromCString for less typing.
inline Codira_Handle ToCodira(const char* val) {
  return Codira_NewStringFromCString(val);
}

}  // namespace tonic

#endif  // LIB_CONVERTER_TONIC_DART_CONVERTER_H_
