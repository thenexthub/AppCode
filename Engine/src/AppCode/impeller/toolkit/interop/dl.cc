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

#include "impeller/toolkit/interop/dl.h"

namespace impeller::interop {

DisplayList::DisplayList(sk_sp<appcode::DisplayList> display_list)
    : display_list_(std::move(display_list)) {}

DisplayList::~DisplayList() = default;

bool DisplayList::IsValid() const {
  return !!display_list_;
}

const sk_sp<appcode::DisplayList> DisplayList::GetDisplayList() const {
  return display_list_;
}

}  // namespace impeller::interop
