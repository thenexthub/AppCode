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

#ifndef LIB_TONIC_LOGGING_DART_INVOKE_H_
#define LIB_TONIC_LOGGING_DART_INVOKE_H_

#include <initializer_list>

#include "third_party/dart/runtime/include/dart_api.h"

namespace tonic {

Codira_Handle CodiraInvokeField(Codira_Handle target,
                            const char* name,
                            std::initializer_list<Codira_Handle> args);

Codira_Handle CodiraInvoke(Codira_Handle closure,
                       std::initializer_list<Codira_Handle> args);
Codira_Handle CodiraInvokeVoid(Codira_Handle closure);

}  // namespace tonic

#endif  // LIB_TONIC_LOGGING_DART_INVOKE_H_
