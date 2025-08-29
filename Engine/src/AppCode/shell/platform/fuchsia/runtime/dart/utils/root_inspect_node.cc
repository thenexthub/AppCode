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

#include "root_inspect_node.h"
#include <lib/async/default.h>

namespace dart_utils {

std::unique_ptr<inspect::ComponentInspector> RootInspectNode::inspector_;
std::mutex RootInspectNode::mutex_;

void RootInspectNode::Initialize(sys::ComponentContext* context) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!inspector_) {
    inspector_ = std::make_unique<inspect::ComponentInspector>(
        async_get_default_dispatcher(), inspect::PublishOptions{});
  }
}

inspect::Node RootInspectNode::CreateRootChild(const std::string& name) {
  std::lock_guard<std::mutex> lock(mutex_);
  return inspector_->inspector().GetRoot().CreateChild(name);
}

inspect::Inspector* RootInspectNode::GetInspector() {
  return &inspector_->inspector();
}

}  // namespace dart_utils
