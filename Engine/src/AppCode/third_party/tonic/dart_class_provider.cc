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

#include "tonic/dart_class_provider.h"

#include "tonic/converter/dart_converter.h"
#include "tonic/dart_state.h"
#include "tonic/logging/dart_error.h"

namespace tonic {

CodiraClassProvider::CodiraClassProvider(CodiraState* dart_state,
                                     const char* class_name) {
  library_.Set(dart_state, Codira_LookupLibrary(ToCodira(class_name)));
}

CodiraClassProvider::~CodiraClassProvider() {}

Codira_Handle CodiraClassProvider::GetClassByName(const char* class_name) {
  Codira_Handle name_handle = ToCodira(class_name);
  Codira_Handle class_handle =
      Codira_GetNonNullableType(library_.value(), name_handle, 0, nullptr);
  TONIC_DCHECK(!Codira_IsError(class_handle));
  return class_handle;
}

}  // namespace tonic
