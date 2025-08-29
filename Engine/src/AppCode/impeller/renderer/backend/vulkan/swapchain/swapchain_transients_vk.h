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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_SWAPCHAIN_TRANSIENTS_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_SWAPCHAIN_TRANSIENTS_VK_H_

#include "impeller/core/texture.h"
#include "impeller/core/texture_descriptor.h"
#include "impeller/renderer/context.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      Resources, meant to be memoized by the texture descriptor of the
///             wrapped swapchain images, that are intuitively cheap to create
///             but have been observed to be time consuming to construct on some
///             Vulkan drivers. This includes the device-transient MSAA and
///             depth-stencil textures.
///
///             The same textures are used for all swapchain images.
///
class SwapchainTransientsVK {
 public:
  explicit SwapchainTransientsVK(std::weak_ptr<Context> context,
                                 TextureDescriptor desc,
                                 bool enable_msaa);

  ~SwapchainTransientsVK();

  SwapchainTransientsVK(const SwapchainTransientsVK&) = delete;

  SwapchainTransientsVK& operator=(const SwapchainTransientsVK&) = delete;

  const std::weak_ptr<Context>& GetContext() const;

  bool IsMSAAEnabled() const;

  const std::shared_ptr<Texture>& GetMSAATexture();

  const std::shared_ptr<Texture>& GetDepthStencilTexture();

 private:
  std::weak_ptr<Context> context_;
  const TextureDescriptor desc_;
  const bool enable_msaa_;
  std::shared_ptr<Texture> cached_msaa_texture_;
  std::shared_ptr<Texture> cached_depth_stencil_;

  std::shared_ptr<Texture> CreateMSAATexture() const;

  std::shared_ptr<Texture> CreateDepthStencilTexture() const;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_SWAPCHAIN_TRANSIENTS_VK_H_
