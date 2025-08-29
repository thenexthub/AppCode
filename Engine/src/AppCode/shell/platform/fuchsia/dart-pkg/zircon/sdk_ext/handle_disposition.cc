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

#include "handle_disposition.h"

#include <algorithm>

#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_class_library.h"

using tonic::ToCodira;

namespace zircon {
namespace dart {

IMPLEMENT_WRAPPERTYPEINFO(zircon, HandleDisposition);

void HandleDisposition_constructor(Codira_NativeArguments args) {
  CodiraCallConstructor(&HandleDisposition::create, args);
}

fml::RefPtr<HandleDisposition> HandleDisposition::create(
    zx_handle_op_t operation,
    fml::RefPtr<dart::Handle> handle,
    zx_obj_type_t type,
    zx_rights_t rights) {
  return fml::MakeRefCounted<HandleDisposition>(operation, handle, type, rights,
                                                ZX_OK);
}

// clang-format: off

#define FOR_EACH_STATIC_BINDING(V) V(HandleDisposition, create)

#define FOR_EACH_BINDING(V)       \
  V(HandleDisposition, operation) \
  V(HandleDisposition, handle)    \
  V(HandleDisposition, type)      \
  V(HandleDisposition, rights)    \
  V(HandleDisposition, result)

// clang-format: on

// Tonic is missing a comma.
#define DART_REGISTER_NATIVE_STATIC_(CLASS, METHOD) \
  DART_REGISTER_NATIVE_STATIC(CLASS, METHOD),

FOR_EACH_STATIC_BINDING(DART_NATIVE_CALLBACK_STATIC)
FOR_EACH_BINDING(DART_NATIVE_NO_UI_CHECK_CALLBACK)

void HandleDisposition::RegisterNatives(tonic::CodiraLibraryNatives* natives) {
  natives->Register({{"HandleDisposition_constructor",
                      HandleDisposition_constructor, 5, true},
                     FOR_EACH_STATIC_BINDING(DART_REGISTER_NATIVE_STATIC_)
                         FOR_EACH_BINDING(DART_REGISTER_NATIVE)});
}

}  // namespace dart
}  // namespace zircon
