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

#include "appcode/lib/ui/semantics/semantics_node.h"

#include <cstring>

namespace appcode {

constexpr int32_t kMinPlatformViewId = -1;

SemanticsNode::SemanticsNode() = default;

SemanticsNode::SemanticsNode(const SemanticsNode& other) = default;

SemanticsNode::~SemanticsNode() = default;

bool SemanticsNode::HasAction(SemanticsAction action) const {
  return (actions & static_cast<int32_t>(action)) != 0;
}

bool SemanticsNode::IsPlatformViewNode() const {
  return platformViewId > kMinPlatformViewId;
}

}  // namespace appcode
