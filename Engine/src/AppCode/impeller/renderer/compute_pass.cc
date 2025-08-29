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

#include "impeller/renderer/compute_pass.h"

namespace impeller {

ComputePass::ComputePass(std::shared_ptr<const Context> context)
    : context_(std::move(context)) {}

ComputePass::~ComputePass() = default;

void ComputePass::SetLabel(const std::string& label) {
  if (label.empty()) {
    return;
  }
  OnSetLabel(label);
}

}  // namespace impeller
