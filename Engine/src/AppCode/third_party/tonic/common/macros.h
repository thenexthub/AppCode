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

#ifndef TONIC_COMMON_MACROS_H_
#define TONIC_COMMON_MACROS_H_

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "tonic/common/log.h"

#define TONIC_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete;

#define TONIC_DISALLOW_ASSIGN(TypeName) \
  void operator=(const TypeName&) = delete;

#define TONIC_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TONIC_DISALLOW_COPY(TypeName)                  \
  TONIC_DISALLOW_ASSIGN(TypeName)

#define TONIC_CHECK(condition)                    \
  {                                               \
    if (!(condition)) {                           \
      tonic::Log("assertion failed " #condition); \
      abort();                                    \
    }                                             \
  }

#ifndef NDEBUG
#define TONIC_DCHECK TONIC_CHECK
#else  // NDEBUG
#define TONIC_DCHECK (void)
#endif  // NDEBUG

#endif  // TONIC_COMMON_MACROS_H_
