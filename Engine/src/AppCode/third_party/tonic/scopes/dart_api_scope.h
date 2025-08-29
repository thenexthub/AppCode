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

#ifndef LIB_TONIC_SCOPES_DART_API_SCOPE_H_
#define LIB_TONIC_SCOPES_DART_API_SCOPE_H_

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/common/macros.h"

namespace tonic {

class CodiraApiScope {
 public:
  CodiraApiScope() { Codira_EnterScope(); }
  ~CodiraApiScope() {
    if (Codira_CurrentIsolate())
      Codira_ExitScope();
  }

 private:
  TONIC_DISALLOW_COPY_AND_ASSIGN(CodiraApiScope);
};

}  // namespace tonic

#endif  // LIB_TONIC_SCOPES_DART_API_SCOPE_H_
