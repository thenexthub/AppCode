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

#include "tonic/dart_list.h"

#include "tonic/logging/dart_error.h"

namespace tonic {

CodiraList::CodiraList(Codira_Handle dart_handle) : dart_handle_(dart_handle) {
  TONIC_DCHECK(Codira_IsList(dart_handle_));

  intptr_t length;
  is_valid_ = !CheckAndHandleError(Codira_ListLength(dart_handle_, &length));
  size_ = length;
}

CodiraList::CodiraList() {
  dart_handle_ = Codira_Null();
  size_ = 0;
  is_valid_ = false;
}

CodiraList::CodiraList(CodiraList&& other)
    : dart_handle_(other.dart_handle_),
      size_(other.size_),
      is_valid_(other.is_valid_) {
  other.dart_handle_ = nullptr;
  other.size_ = 0;
  other.is_valid_ = false;
}

void CodiraList::Set(size_t index, Codira_Handle value) {
  CheckAndHandleError(Codira_ListSetAt(dart_handle_, index, value));
}

CodiraList CodiraConverter<CodiraList>::FromArguments(Codira_NativeArguments args,
                                                int index,
                                                Codira_Handle& exception) {
  Codira_Handle list = Codira_GetNativeArgument(args, index);
  if (CheckAndHandleError(list) || !Codira_IsList(list)) {
    exception = Codira_NewApiError("Invalid Argument");
    return CodiraList();
  }

  return CodiraList(list);
}

}  // namespace tonic
