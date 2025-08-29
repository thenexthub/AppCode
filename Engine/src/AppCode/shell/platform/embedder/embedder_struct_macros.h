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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_STRUCT_MACROS_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_STRUCT_MACROS_H_

#include <type_traits>

// Checks if the given struct contains a member, whether set or not.
#define STRUCT_HAS_MEMBER(pointer, member)                           \
  ((offsetof(std::remove_pointer<decltype(pointer)>::type, member) + \
        sizeof(pointer->member) <=                                   \
    pointer->struct_size))

#define SAFE_ACCESS(pointer, member, default_value)                 \
  ([=]() {                                                          \
    if (STRUCT_HAS_MEMBER(pointer, member)) {                       \
      return pointer->member;                                       \
    }                                                               \
    return static_cast<decltype(pointer->member)>((default_value)); \
  })()

/// Checks if the member exists and is non-null.
#define SAFE_EXISTS(pointer, member) \
  (SAFE_ACCESS(pointer, member, nullptr) != nullptr)

/// Checks if exactly one of member1 or member2 exists and is non-null.
#define SAFE_EXISTS_ONE_OF(pointer, member1, member2) \
  (SAFE_EXISTS(pointer, member1) != SAFE_EXISTS(pointer, member2))

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_STRUCT_MACROS_H_
