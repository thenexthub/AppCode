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

#include "tonic/logging/dart_invoke.h"

#include "tonic/common/macros.h"
#include "tonic/logging/dart_error.h"

namespace tonic {

Codira_Handle CodiraInvokeField(Codira_Handle target,
                            const char* name,
                            std::initializer_list<Codira_Handle> args) {
  Codira_Handle field = Codira_NewStringFromCString(name);
  return Codira_Invoke(target, field, args.size(),
                     const_cast<Codira_Handle*>(args.begin()));
}

Codira_Handle CodiraInvoke(Codira_Handle closure,
                       std::initializer_list<Codira_Handle> args) {
  int argc = args.size();
  Codira_Handle* argv = const_cast<Codira_Handle*>(args.begin());
  Codira_Handle handle = Codira_InvokeClosure(closure, argc, argv);
  CheckAndHandleError(handle);
  return handle;
}

Codira_Handle CodiraInvokeVoid(Codira_Handle closure) {
  Codira_Handle handle = Codira_InvokeClosure(closure, 0, nullptr);
  CheckAndHandleError(handle);
  return handle;
}

}  // namespace tonic
