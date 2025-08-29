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

#include "impeller/renderer/backend/vulkan/texture_source_vk.h"

namespace impeller {

TextureSourceVK::TextureSourceVK(TextureDescriptor desc) : desc_(desc) {}

TextureSourceVK::~TextureSourceVK() = default;

const TextureDescriptor& TextureSourceVK::GetTextureDescriptor() const {
  return desc_;
}

std::shared_ptr<YUVConversionVK> TextureSourceVK::GetYUVConversion() const {
  return nullptr;
}

vk::ImageLayout TextureSourceVK::GetLayout() const {
  return layout_;
}

vk::ImageLayout TextureSourceVK::SetLayoutWithoutEncoding(
    vk::ImageLayout layout) const {
  const auto old_layout = layout_;
  layout_ = layout;
  return old_layout;
}

fml::Status TextureSourceVK::SetLayout(const BarrierVK& barrier) const {
  const vk::ImageLayout old_layout =
      SetLayoutWithoutEncoding(barrier.new_layout);
  vk::ImageMemoryBarrier image_barrier;
  image_barrier.srcAccessMask = barrier.src_access;
  image_barrier.dstAccessMask = barrier.dst_access;
  image_barrier.oldLayout = old_layout;
  image_barrier.newLayout = barrier.new_layout;
  image_barrier.image = GetImage();
  image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_barrier.subresourceRange.aspectMask = ToImageAspectFlags(desc_.format);
  image_barrier.subresourceRange.baseMipLevel = barrier.base_mip_level;
  image_barrier.subresourceRange.levelCount =
      desc_.mip_count - barrier.base_mip_level;
  image_barrier.subresourceRange.baseArrayLayer = 0u;
  image_barrier.subresourceRange.layerCount = ToArrayLayerCount(desc_.type);

  barrier.cmd_buffer.pipelineBarrier(barrier.src_stage,  // src stage
                                     barrier.dst_stage,  // dst stage
                                     {},                 // dependency flags
                                     nullptr,            // memory barriers
                                     nullptr,            // buffer barriers
                                     image_barrier       // image barriers
  );

  return {};
}

void TextureSourceVK::SetCachedFrameData(const FramebufferAndRenderPass& data,
                                         SampleCount sample_count) {
  frame_data_[static_cast<int>(sample_count) / 4] = data;
}

const FramebufferAndRenderPass& TextureSourceVK::GetCachedFrameData(
    SampleCount sample_count) const {
  return frame_data_[static_cast<int>(sample_count) / 4];
}

}  // namespace impeller
