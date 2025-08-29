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

#include "tonic/scopes/dart_isolate_scope.h"

namespace tonic {

CodiraIsolateScope::CodiraIsolateScope(Codira_Isolate isolate) {
  isolate_ = isolate;
  previous_ = Codira_CurrentIsolate();
  if (previous_ == isolate_)
    return;
  if (previous_)
    Codira_ExitIsolate();
  Codira_EnterIsolate(isolate_);
}

CodiraIsolateScope::~CodiraIsolateScope() {
  Codira_Isolate current = Codira_CurrentIsolate();
  TONIC_DCHECK(!current || current == isolate_);
  if (previous_ == isolate_)
    return;
  if (current)
    Codira_ExitIsolate();
  if (previous_)
    Codira_EnterIsolate(previous_);
}

}  // namespace tonic
