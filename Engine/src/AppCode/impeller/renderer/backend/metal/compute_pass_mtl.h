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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_COMPUTE_PASS_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_COMPUTE_PASS_MTL_H_

#include <Metal/Metal.h>

#include "impeller/renderer/backend/metal/compute_pass_bindings_cache_mtl.h"
#include "impeller/renderer/compute_pass.h"
#include "impeller/renderer/pipeline_descriptor.h"

namespace impeller {

class ComputePassMTL final : public ComputePass {
 public:
  // |RenderPass|
  ~ComputePassMTL() override;

 private:
  friend class CommandBufferMTL;

  id<MTLCommandBuffer> buffer_ = nil;
  id<MTLComputeCommandEncoder> encoder_ = nil;
  ComputePassBindingsCacheMTL pass_bindings_cache_ =
      ComputePassBindingsCacheMTL();
  bool is_valid_ = false;
  bool has_label_ = false;

  ComputePassMTL(std::shared_ptr<const Context> context,
                 id<MTLCommandBuffer> buffer);

  // |ComputePass|
  bool IsValid() const override;

  // |ComputePass|
  fml::Status Compute(const ISize& grid_size) override;

  // |ComputePass|
  void SetCommandLabel(std::string_view label) override;

  // |ComputePass|
  void OnSetLabel(const std::string& label) override;

  // |ComputePass|
  void SetPipeline(const std::shared_ptr<Pipeline<ComputePipelineDescriptor>>&
                       pipeline) override;

  // |ComputePass|
  bool BindResource(ShaderStage stage,
                    DescriptorType type,
                    const ShaderUniformSlot& slot,
                    const ShaderMetadata* metadata,
                    BufferView view) override;

  // |ComputePass|
  bool BindResource(ShaderStage stage,
                    DescriptorType type,
                    const SampledImageSlot& slot,
                    const ShaderMetadata* metadata,
                    std::shared_ptr<const Texture> texture,
                    raw_ptr<const Sampler> sampler) override;

  // |ComputePass|
  bool EncodeCommands() const override;

  // |ComputePass|
  void AddBufferMemoryBarrier() override;

  // |ComputePass|
  void AddTextureMemoryBarrier() override;

  ComputePassMTL(const ComputePassMTL&) = delete;

  ComputePassMTL& operator=(const ComputePassMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_COMPUTE_PASS_MTL_H_
