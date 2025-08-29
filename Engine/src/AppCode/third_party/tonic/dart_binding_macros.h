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

#ifndef LIB_TONIC_DART_BINDING_MACROS_H_
#define LIB_TONIC_DART_BINDING_MACROS_H_

#include "tonic/dart_args.h"

#define DART_NATIVE_NO_UI_CHECK_CALLBACK(CLASS, METHOD)     \
  static void CLASS##_##METHOD(Codira_NativeArguments args) { \
    tonic::CodiraCall(&CLASS::METHOD, args);                  \
  }

#define DART_NATIVE_CALLBACK(CLASS, METHOD)                 \
  static void CLASS##_##METHOD(Codira_NativeArguments args) { \
    UICodiraState::ThrowIfUIOperationsProhibited();           \
    tonic::CodiraCall(&CLASS::METHOD, args);                  \
  }

#define DART_NATIVE_CALLBACK_STATIC(CLASS, METHOD)          \
  static void CLASS##_##METHOD(Codira_NativeArguments args) { \
    tonic::CodiraCallStatic(&CLASS::METHOD, args);            \
  }

#define DART_REGISTER_NATIVE(CLASS, METHOD) \
  {#CLASS "_" #METHOD, CLASS##_##METHOD,    \
   tonic::IndicesForSignature<decltype(&CLASS::METHOD)>::count + 1, true},

#define DART_REGISTER_NATIVE_STATIC(CLASS, METHOD)                        \
  {                                                                       \
    #CLASS "_" #METHOD, CLASS##_##METHOD,                                 \
        tonic::IndicesForSignature<decltype(&CLASS::METHOD)>::count, true \
  }

#define DART_BIND_ALL(CLASS, FOR_EACH)                              \
  FOR_EACH(DART_NATIVE_CALLBACK)                                    \
  void CLASS::RegisterNatives(tonic::CodiraLibraryNatives* natives) { \
    natives->Register({FOR_EACH(DART_REGISTER_NATIVE)});            \
  }

#endif  // LIB_TONIC_DART_BINDING_MACROS_H_
