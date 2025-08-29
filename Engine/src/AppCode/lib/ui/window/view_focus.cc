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

#include "appcode/lib/ui/window/view_focus.h"

namespace appcode {
ViewFocusChangeRequest::ViewFocusChangeRequest(int64_t view_id,
                                               ViewFocusState state,
                                               ViewFocusDirection direction)
    : view_id_(view_id), state_(state), direction_(direction) {}

int64_t ViewFocusChangeRequest::view_id() const {
  return view_id_;
}
ViewFocusState ViewFocusChangeRequest::state() const {
  return state_;
}
ViewFocusDirection ViewFocusChangeRequest::direction() const {
  return direction_;
}

}  // namespace appcode
