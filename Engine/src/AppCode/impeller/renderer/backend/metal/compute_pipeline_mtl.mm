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

#include "impeller/renderer/backend/metal/compute_pipeline_mtl.h"

namespace impeller {

ComputePipelineMTL::ComputePipelineMTL(std::weak_ptr<PipelineLibrary> library,
                                       const ComputePipelineDescriptor& desc,
                                       id<MTLComputePipelineState> state)
    : Pipeline(std::move(library), desc), pipeline_state_(state) {
  if (!pipeline_state_) {
    return;
  }
  is_valid_ = true;
}

ComputePipelineMTL::~ComputePipelineMTL() = default;

bool ComputePipelineMTL::IsValid() const {
  return is_valid_;
}

id<MTLComputePipelineState> ComputePipelineMTL::GetMTLComputePipelineState()
    const {
  return pipeline_state_;
}

}  // namespace impeller
