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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_RUNTIME_EFFECT_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_RUNTIME_EFFECT_CONTENTS_H_

#include <memory>
#include <vector>

#include "impeller/core/host_buffer.h"
#include "impeller/core/sampler_descriptor.h"
#include "impeller/entity/contents/color_source_contents.h"
#include "impeller/runtime_stage/runtime_stage.h"

namespace impeller {

class RuntimeEffectContents final : public ColorSourceContents {
 public:
  struct TextureInput {
    SamplerDescriptor sampler_descriptor;
    std::shared_ptr<Texture> texture;
  };

  void SetRuntimeStage(std::shared_ptr<RuntimeStage> runtime_stage);

  void SetUniformData(std::shared_ptr<std::vector<uint8_t>> uniform_data);

  void SetTextureInputs(std::vector<TextureInput> texture_inputs);

  // |Contents|
  bool Render(const ContentContext& renderer,
              const Entity& entity,
              RenderPass& pass) const override;

  /// Load the runtime effect and ensure a default PSO is initialized.
  bool BootstrapShader(const ContentContext& renderer) const;

  // Visible for testing
  static BufferView EmplaceVulkanUniform(
      const std::shared_ptr<const std::vector<uint8_t>>& input_data,
      HostBuffer& host_buffer,
      const RuntimeUniformDescription& uniform,
      size_t minimum_uniform_alignment);

 private:
  bool RegisterShader(const ContentContext& renderer) const;

  // If async is true, this will always return nullptr as pipeline creation
  // is not blocked on.
  std::shared_ptr<Pipeline<PipelineDescriptor>> CreatePipeline(
      const ContentContext& renderer,
      ContentContextOptions options,
      bool async) const;

  std::shared_ptr<RuntimeStage> runtime_stage_;
  std::shared_ptr<std::vector<uint8_t>> uniform_data_;
  std::vector<TextureInput> texture_inputs_;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_RUNTIME_EFFECT_CONTENTS_H_
