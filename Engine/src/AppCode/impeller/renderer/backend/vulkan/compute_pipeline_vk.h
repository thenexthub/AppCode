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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_COMPUTE_PIPELINE_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_COMPUTE_PIPELINE_VK_H_

#include <memory>

#include "impeller/base/backend_cast.h"
#include "impeller/renderer/backend/vulkan/device_holder_vk.h"
#include "impeller/renderer/backend/vulkan/vk.h"
#include "impeller/renderer/pipeline.h"

namespace impeller {

class ComputePipelineVK final
    : public Pipeline<ComputePipelineDescriptor>,
      public BackendCast<ComputePipelineVK,
                         Pipeline<ComputePipelineDescriptor>> {
 public:
  ComputePipelineVK(std::weak_ptr<DeviceHolderVK> device_holder,
                    std::weak_ptr<PipelineLibrary> library,
                    const ComputePipelineDescriptor& desc,
                    vk::UniquePipeline pipeline,
                    vk::UniquePipelineLayout layout,
                    vk::UniqueDescriptorSetLayout descriptor_set_layout,
                    PipelineKey pipeline_key);

  // |Pipeline|
  ~ComputePipelineVK() override;

  const vk::Pipeline& GetPipeline() const;

  const vk::PipelineLayout& GetPipelineLayout() const;

  const vk::DescriptorSetLayout& GetDescriptorSetLayout() const;

  /// @brief Retrieve the unique identifier for this pipeline's descriptor set
  ///        layout.
  PipelineKey GetPipelineKey() const;

 private:
  friend class PipelineLibraryVK;

  std::weak_ptr<DeviceHolderVK> device_holder_;
  vk::UniquePipeline pipeline_;
  vk::UniquePipelineLayout layout_;
  vk::UniqueDescriptorSetLayout descriptor_set_layout_;
  const PipelineKey pipeline_key_;
  bool is_valid_ = false;

  // |Pipeline|
  bool IsValid() const override;

  ComputePipelineVK(const ComputePipelineVK&) = delete;

  ComputePipelineVK& operator=(const ComputePipelineVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_COMPUTE_PIPELINE_VK_H_
