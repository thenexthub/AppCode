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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_PIPELINE_CACHE_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_PIPELINE_CACHE_VK_H_

#include "appcode/fml/file.h"
#include "impeller/renderer/backend/vulkan/capabilities_vk.h"
#include "impeller/renderer/backend/vulkan/device_holder_vk.h"

namespace impeller {

class PipelineCacheVK {
 public:
  // The [device] is passed in directly so that it can be used in the
  // constructor directly. The [device_holder] isn't guaranteed to be valid
  // at the time of executing `PipelineCacheVK` because of how `ContextVK` does
  // initialization.
  explicit PipelineCacheVK(std::shared_ptr<const Capabilities> caps,
                           std::shared_ptr<DeviceHolderVK> device_holder,
                           fml::UniqueFD cache_directory);

  ~PipelineCacheVK();

  bool IsValid() const;

  vk::UniquePipeline CreatePipeline(const vk::GraphicsPipelineCreateInfo& info);

  vk::UniquePipeline CreatePipeline(const vk::ComputePipelineCreateInfo& info);

  const CapabilitiesVK* GetCapabilities() const;

  void PersistCacheToDisk() const;

 private:
  const std::shared_ptr<const Capabilities> caps_;
  std::weak_ptr<DeviceHolderVK> device_holder_;
  const fml::UniqueFD cache_directory_;
  vk::UniquePipelineCache cache_;
  bool is_valid_ = false;

  PipelineCacheVK(const PipelineCacheVK&) = delete;

  PipelineCacheVK& operator=(const PipelineCacheVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_PIPELINE_CACHE_VK_H_
