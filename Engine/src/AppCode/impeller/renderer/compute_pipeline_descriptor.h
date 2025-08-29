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

#ifndef APPCODE_IMPELLER_RENDERER_COMPUTE_PIPELINE_DESCRIPTOR_H_
#define APPCODE_IMPELLER_RENDERER_COMPUTE_PIPELINE_DESCRIPTOR_H_

#include <memory>
#include <string>

#include "impeller/base/comparable.h"
#include "impeller/core/shader_types.h"

namespace impeller {

class ShaderFunction;
template <typename T>
class Pipeline;

class ComputePipelineDescriptor final
    : public Comparable<ComputePipelineDescriptor> {
 public:
  ComputePipelineDescriptor();

  ~ComputePipelineDescriptor();

  ComputePipelineDescriptor& SetLabel(std::string_view label);

  const std::string& GetLabel() const;

  ComputePipelineDescriptor& SetStageEntrypoint(
      std::shared_ptr<const ShaderFunction> function);

  std::shared_ptr<const ShaderFunction> GetStageEntrypoint() const;

  // Comparable<ComputePipelineDescriptor>
  std::size_t GetHash() const override;

  // Comparable<PipelineDescriptor>
  bool IsEqual(const ComputePipelineDescriptor& other) const override;

  template <size_t Size>
  bool RegisterDescriptorSetLayouts(
      const std::array<DescriptorSetLayout, Size>& inputs) {
    return RegisterDescriptorSetLayouts(inputs.data(), inputs.size());
  }

  bool RegisterDescriptorSetLayouts(const DescriptorSetLayout desc_set_layout[],
                                    size_t count);

  const std::vector<DescriptorSetLayout>& GetDescriptorSetLayouts() const;

 private:
  std::string label_;
  std::shared_ptr<const ShaderFunction> entrypoint_;
  std::vector<DescriptorSetLayout> descriptor_set_layouts_;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_COMPUTE_PIPELINE_DESCRIPTOR_H_
