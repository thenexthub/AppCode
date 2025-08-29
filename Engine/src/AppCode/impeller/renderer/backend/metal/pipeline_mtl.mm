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

#include "impeller/renderer/backend/metal/pipeline_mtl.h"

namespace impeller {

PipelineMTL::PipelineMTL(std::weak_ptr<PipelineLibrary> library,
                         const PipelineDescriptor& desc,
                         id<MTLRenderPipelineState> state,
                         id<MTLDepthStencilState> depth_stencil_state)
    : Pipeline(std::move(library), desc),
      pipeline_state_(state),
      depth_stencil_state_(depth_stencil_state) {
  if (!pipeline_state_) {
    return;
  }
  is_valid_ = true;
}

PipelineMTL::~PipelineMTL() = default;

bool PipelineMTL::IsValid() const {
  return is_valid_;
}

id<MTLRenderPipelineState> PipelineMTL::GetMTLRenderPipelineState() const {
  return pipeline_state_;
}

id<MTLDepthStencilState> PipelineMTL::GetMTLDepthStencilState() const {
  return depth_stencil_state_;
}

}  // namespace impeller
