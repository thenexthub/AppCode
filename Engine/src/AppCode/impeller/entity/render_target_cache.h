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

#ifndef APPCODE_IMPELLER_ENTITY_RENDER_TARGET_CACHE_H_
#define APPCODE_IMPELLER_ENTITY_RENDER_TARGET_CACHE_H_

#include <string_view>

#include "impeller/renderer/render_target.h"

namespace impeller {

/// @brief An implementation of the [RenderTargetAllocator] that caches all
///        allocated texture data for one frame.
///
///        Any textures unused after a frame are immediately discarded.
class RenderTargetCache : public RenderTargetAllocator {
 public:
  explicit RenderTargetCache(std::shared_ptr<Allocator> allocator,
                             uint32_t keep_alive_frame_count = 4);

  ~RenderTargetCache() = default;

  // |RenderTargetAllocator|
  void Start() override;

  // |RenderTargetAllocator|
  void End() override;

  // |RenderTargetAllocator|
  void DisableCache() override;

  // |RenderTargetAllocator|
  void EnableCache() override;

  RenderTarget CreateOffscreen(
      const Context& context,
      ISize size,
      int mip_count,
      std::string_view label = "Offscreen",
      RenderTarget::AttachmentConfig color_attachment_config =
          RenderTarget::kDefaultColorAttachmentConfig,
      std::optional<RenderTarget::AttachmentConfig> stencil_attachment_config =
          RenderTarget::kDefaultStencilAttachmentConfig,
      const std::shared_ptr<Texture>& existing_color_texture = nullptr,
      const std::shared_ptr<Texture>& existing_depth_stencil_texture =
          nullptr) override;

  RenderTarget CreateOffscreenMSAA(
      const Context& context,
      ISize size,
      int mip_count,
      std::string_view label = "Offscreen MSAA",
      RenderTarget::AttachmentConfigMSAA color_attachment_config =
          RenderTarget::kDefaultColorAttachmentConfigMSAA,
      std::optional<RenderTarget::AttachmentConfig> stencil_attachment_config =
          RenderTarget::kDefaultStencilAttachmentConfig,
      const std::shared_ptr<Texture>& existing_color_msaa_texture = nullptr,
      const std::shared_ptr<Texture>& existing_color_resolve_texture = nullptr,
      const std::shared_ptr<Texture>& existing_depth_stencil_texture =
          nullptr) override;

  // visible for testing.
  size_t CachedTextureCount() const;

 private:
  struct RenderTargetData {
    bool used_this_frame;
    uint32_t keep_alive_frame_count;
    RenderTargetConfig config;
    RenderTarget render_target;
  };

  bool CacheEnabled() const;

  std::vector<RenderTargetData> render_target_data_;
  uint32_t keep_alive_frame_count_;
  uint32_t cache_disabled_count_ = 0;

  RenderTargetCache(const RenderTargetCache&) = delete;

  RenderTargetCache& operator=(const RenderTargetCache&) = delete;

 public:
  /// Visible for testing.
  std::vector<RenderTargetData>::const_iterator GetRenderTargetDataBegin()
      const {
    return render_target_data_.begin();
  }

  /// Visible for testing.
  std::vector<RenderTargetData>::const_iterator GetRenderTargetDataEnd() const {
    return render_target_data_.end();
  }
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_RENDER_TARGET_CACHE_H_
