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

#include "impeller/renderer/backend/metal/compute_pass_bindings_cache_mtl.h"

namespace impeller {

void ComputePassBindingsCacheMTL::SetComputePipelineState(
    id<MTLComputePipelineState> pipeline) {
  if (pipeline == pipeline_) {
    return;
  }
  pipeline_ = pipeline;
  [encoder_ setComputePipelineState:pipeline_];
}

id<MTLComputePipelineState> ComputePassBindingsCacheMTL::GetPipeline() const {
  return pipeline_;
}

void ComputePassBindingsCacheMTL::SetEncoder(
    id<MTLComputeCommandEncoder> encoder) {
  encoder_ = encoder;
}

void ComputePassBindingsCacheMTL::SetBuffer(uint64_t index,
                                            uint64_t offset,
                                            id<MTLBuffer> buffer) {
  auto found = buffers_.find(index);
  if (found != buffers_.end() && found->second.buffer == buffer) {
    // The right buffer is bound. Check if its offset needs to be updated.
    if (found->second.offset == offset) {
      // Buffer and its offset is identical. Nothing to do.
      return;
    }

    // Only the offset needs to be updated.
    found->second.offset = offset;

    [encoder_ setBufferOffset:offset atIndex:index];
    return;
  }

  buffers_[index] = {buffer, static_cast<size_t>(offset)};
  [encoder_ setBuffer:buffer offset:offset atIndex:index];
}

void ComputePassBindingsCacheMTL::SetTexture(uint64_t index,
                                             id<MTLTexture> texture) {
  auto found = textures_.find(index);
  if (found != textures_.end() && found->second == texture) {
    // Already bound.
    return;
  }
  textures_[index] = texture;
  [encoder_ setTexture:texture atIndex:index];
  return;
}

void ComputePassBindingsCacheMTL::SetSampler(uint64_t index,
                                             id<MTLSamplerState> sampler) {
  auto found = samplers_.find(index);
  if (found != samplers_.end() && found->second == sampler) {
    // Already bound.
    return;
  }
  samplers_[index] = sampler;
  [encoder_ setSamplerState:sampler atIndex:index];
  return;
}

}  // namespace impeller
