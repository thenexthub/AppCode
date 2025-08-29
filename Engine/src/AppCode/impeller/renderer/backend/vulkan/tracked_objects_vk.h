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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_TRACKED_OBJECTS_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_TRACKED_OBJECTS_VK_H_

#include <memory>

#include "impeller/renderer/backend/vulkan/context_vk.h"
#include "impeller/renderer/backend/vulkan/descriptor_pool_vk.h"
#include "impeller/renderer/backend/vulkan/gpu_tracer_vk.h"
#include "impeller/renderer/backend/vulkan/texture_source_vk.h"

namespace impeller {

/// @brief A per-frame object used to track resource lifetimes and allocate
///        command buffers and descriptor sets.
class TrackedObjectsVK {
 public:
  explicit TrackedObjectsVK(const std::weak_ptr<const ContextVK>& context,
                            const std::shared_ptr<CommandPoolVK>& pool,
                            std::shared_ptr<DescriptorPoolVK> descriptor_pool,
                            std::unique_ptr<GPUProbe> probe);

  ~TrackedObjectsVK();

  bool IsValid() const;

  void Track(const std::shared_ptr<SharedObjectVK>& object);

  void Track(const std::shared_ptr<const DeviceBuffer>& buffer);

  void Track(const std::shared_ptr<const TextureSourceVK>& texture);

  vk::CommandBuffer GetCommandBuffer() const;

  DescriptorPoolVK& GetDescriptorPool();

  GPUProbe& GetGPUProbe() const;

 private:
  std::shared_ptr<DescriptorPoolVK> desc_pool_;
  // `shared_ptr` since command buffers have a link to the command pool.
  std::shared_ptr<CommandPoolVK> pool_;
  vk::UniqueCommandBuffer buffer_;
  std::vector<std::shared_ptr<SharedObjectVK>> tracked_objects_;
  std::vector<std::shared_ptr<const DeviceBuffer>> tracked_buffers_;
  std::vector<std::shared_ptr<const TextureSourceVK>> tracked_textures_;
  std::unique_ptr<GPUProbe> probe_;
  bool is_valid_ = false;

  TrackedObjectsVK(const TrackedObjectsVK&) = delete;

  TrackedObjectsVK& operator=(const TrackedObjectsVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_TRACKED_OBJECTS_VK_H_
