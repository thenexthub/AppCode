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

#include "impeller/renderer/backend/vulkan/tracked_objects_vk.h"

#include "impeller/renderer/backend/vulkan/command_pool_vk.h"
#include "impeller/renderer/backend/vulkan/gpu_tracer_vk.h"

namespace impeller {

TrackedObjectsVK::TrackedObjectsVK(
    const std::weak_ptr<const ContextVK>& context,
    const std::shared_ptr<CommandPoolVK>& pool,
    std::shared_ptr<DescriptorPoolVK> descriptor_pool,
    std::unique_ptr<GPUProbe> probe)
    : desc_pool_(std::move(descriptor_pool)), probe_(std::move(probe)) {
  if (!pool) {
    return;
  }
  auto buffer = pool->CreateCommandBuffer();
  if (!buffer) {
    return;
  }
  pool_ = pool;
  buffer_ = std::move(buffer);
  is_valid_ = true;
  // Starting values were selected by looking at values from
  // AiksTest.CanRenderMultipleBackdropBlurWithSingleBackdropId.
  tracked_objects_.reserve(5);
  tracked_buffers_.reserve(5);
  tracked_textures_.reserve(5);
}

TrackedObjectsVK::~TrackedObjectsVK() {
  if (!buffer_) {
    return;
  }
  pool_->CollectCommandBuffer(std::move(buffer_));
}

bool TrackedObjectsVK::IsValid() const {
  return is_valid_;
}

void TrackedObjectsVK::Track(const std::shared_ptr<SharedObjectVK>& object) {
  if (!object || (!tracked_objects_.empty() &&
                  object.get() == tracked_objects_.back().get())) {
    return;
  }
  tracked_objects_.emplace_back(object);
}

void TrackedObjectsVK::Track(
    const std::shared_ptr<const DeviceBuffer>& buffer) {
  if (!buffer || (!tracked_buffers_.empty() &&
                  buffer.get() == tracked_buffers_.back().get())) {
    return;
  }
  tracked_buffers_.emplace_back(buffer);
}

void TrackedObjectsVK::Track(
    const std::shared_ptr<const TextureSourceVK>& texture) {
  if (!texture || (!tracked_textures_.empty() &&
                   texture.get() == tracked_textures_.back().get())) {
    return;
  }
  tracked_textures_.emplace_back(texture);
}

vk::CommandBuffer TrackedObjectsVK::GetCommandBuffer() const {
  return *buffer_;
}

DescriptorPoolVK& TrackedObjectsVK::GetDescriptorPool() {
  return *desc_pool_;
}

GPUProbe& TrackedObjectsVK::GetGPUProbe() const {
  return *probe_.get();
}

}  // namespace impeller
