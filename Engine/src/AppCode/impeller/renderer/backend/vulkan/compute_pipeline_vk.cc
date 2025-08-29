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

#include "impeller/renderer/backend/vulkan/compute_pipeline_vk.h"

namespace impeller {

ComputePipelineVK::ComputePipelineVK(
    std::weak_ptr<DeviceHolderVK> device_holder,
    std::weak_ptr<PipelineLibrary> library,
    const ComputePipelineDescriptor& desc,
    vk::UniquePipeline pipeline,
    vk::UniquePipelineLayout layout,
    vk::UniqueDescriptorSetLayout descriptor_set_layout,
    PipelineKey pipeline_key)
    : Pipeline(std::move(library), desc),
      device_holder_(std::move(device_holder)),
      pipeline_(std::move(pipeline)),
      layout_(std::move(layout)),
      descriptor_set_layout_(std::move(descriptor_set_layout)),
      pipeline_key_(pipeline_key) {
  is_valid_ = pipeline_ && layout_ && descriptor_set_layout_;
}

ComputePipelineVK::~ComputePipelineVK() {
  std::shared_ptr<DeviceHolderVK> device_holder = device_holder_.lock();
  if (device_holder) {
    descriptor_set_layout_.reset();
    layout_.reset();
    pipeline_.reset();
  } else {
    descriptor_set_layout_.release();
    layout_.release();
    pipeline_.release();
  }
}

bool ComputePipelineVK::IsValid() const {
  return is_valid_;
}

const vk::Pipeline& ComputePipelineVK::GetPipeline() const {
  return *pipeline_;
}

const vk::PipelineLayout& ComputePipelineVK::GetPipelineLayout() const {
  return *layout_;
}

const vk::DescriptorSetLayout& ComputePipelineVK::GetDescriptorSetLayout()
    const {
  return *descriptor_set_layout_;
}

PipelineKey ComputePipelineVK::GetPipelineKey() const {
  return pipeline_key_;
}

}  // namespace impeller
