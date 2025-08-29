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

#ifndef APPCODE_IMPELLER_RENDERER_VERTEX_DESCRIPTOR_H_
#define APPCODE_IMPELLER_RENDERER_VERTEX_DESCRIPTOR_H_

#include <vector>

#include "impeller/base/comparable.h"
#include "impeller/core/shader_types.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief        Describes the format and layout of vertices expected by the
///               pipeline. While it is possible to construct these descriptors
///               manually, it would be tedious to do so. These are usually
///               constructed using shader information reflected using
///               `impellerc`. The usage of this class is indirectly via
///               `PipelineBuilder<VS, FS>`.
///
class VertexDescriptor final : public Comparable<VertexDescriptor> {
 public:
  static constexpr size_t kReservedVertexBufferIndex =
      30u;  // The final slot available. Regular buffer indices go up from 0.

  VertexDescriptor();

  // |Comparable<PipelineVertexDescriptor>|
  virtual ~VertexDescriptor();

  template <size_t Size, size_t LayoutSize>
  void SetStageInputs(
      const std::array<const ShaderStageIOSlot*, Size>& inputs,
      const std::array<const ShaderStageBufferLayout*, LayoutSize>& layout) {
    return SetStageInputs(inputs.data(), inputs.size(), layout.data(),
                          layout.size());
  }

  void SetStageInputs(const std::vector<ShaderStageIOSlot>& inputs,
                      const std::vector<ShaderStageBufferLayout>& layout);

  template <size_t Size>
  void RegisterDescriptorSetLayouts(
      const std::array<DescriptorSetLayout, Size>& inputs) {
    return RegisterDescriptorSetLayouts(inputs.data(), inputs.size());
  }

  void SetStageInputs(const ShaderStageIOSlot* const stage_inputs[],
                      size_t count,
                      const ShaderStageBufferLayout* const stage_layout[],
                      size_t layout_count);

  void RegisterDescriptorSetLayouts(const DescriptorSetLayout desc_set_layout[],
                                    size_t count);

  const std::vector<ShaderStageIOSlot>& GetStageInputs() const;

  const std::vector<ShaderStageBufferLayout>& GetStageLayouts() const;

  const std::vector<DescriptorSetLayout>& GetDescriptorSetLayouts() const;

  // |Comparable<VertexDescriptor>|
  std::size_t GetHash() const override;

  // |Comparable<VertexDescriptor>|
  bool IsEqual(const VertexDescriptor& other) const override;

  bool UsesInputAttacments() const;

 private:
  std::vector<ShaderStageIOSlot> inputs_;
  std::vector<ShaderStageBufferLayout> layouts_;
  std::vector<DescriptorSetLayout> desc_set_layouts_;
  bool uses_input_attachments_ = false;

  VertexDescriptor(const VertexDescriptor&) = delete;

  VertexDescriptor& operator=(const VertexDescriptor&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_VERTEX_DESCRIPTOR_H_
