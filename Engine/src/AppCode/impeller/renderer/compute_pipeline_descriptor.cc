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

#include "impeller/renderer/compute_pipeline_descriptor.h"

#include "impeller/core/formats.h"
#include "impeller/renderer/shader_function.h"
#include "impeller/renderer/shader_library.h"
#include "impeller/renderer/vertex_descriptor.h"

namespace impeller {

ComputePipelineDescriptor::ComputePipelineDescriptor() = default;

ComputePipelineDescriptor::~ComputePipelineDescriptor() = default;

// Comparable<ComputePipelineDescriptor>
std::size_t ComputePipelineDescriptor::GetHash() const {
  auto seed = fml::HashCombine();
  fml::HashCombineSeed(seed, label_);
  if (entrypoint_) {
    fml::HashCombineSeed(seed, entrypoint_->GetHash());
  }
  return seed;
}

// Comparable<ComputePipelineDescriptor>
bool ComputePipelineDescriptor::IsEqual(
    const ComputePipelineDescriptor& other) const {
  return label_ == other.label_ &&
         DeepComparePointer(entrypoint_, other.entrypoint_);
}

ComputePipelineDescriptor& ComputePipelineDescriptor::SetLabel(
    std::string_view label) {
  label_ = label;
  return *this;
}

ComputePipelineDescriptor& ComputePipelineDescriptor::SetStageEntrypoint(
    std::shared_ptr<const ShaderFunction> function) {
  FML_DCHECK(!function || function->GetStage() == ShaderStage::kCompute);
  if (!function || function->GetStage() != ShaderStage::kCompute) {
    return *this;
  }

  if (function->GetStage() == ShaderStage::kUnknown) {
    return *this;
  }

  entrypoint_ = std::move(function);

  return *this;
}

std::shared_ptr<const ShaderFunction>
ComputePipelineDescriptor::GetStageEntrypoint() const {
  return entrypoint_;
}

const std::string& ComputePipelineDescriptor::GetLabel() const {
  return label_;
}

bool ComputePipelineDescriptor::RegisterDescriptorSetLayouts(
    const DescriptorSetLayout desc_set_layout[],
    size_t count) {
  descriptor_set_layouts_.reserve(descriptor_set_layouts_.size() + count);
  for (size_t i = 0; i < count; i++) {
    descriptor_set_layouts_.emplace_back(desc_set_layout[i]);
  }
  return true;
}

const std::vector<DescriptorSetLayout>&
ComputePipelineDescriptor::GetDescriptorSetLayouts() const {
  return descriptor_set_layouts_;
}

}  // namespace impeller
