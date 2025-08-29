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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_SURFACE_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_SURFACE_VK_H_

#include <memory>

#include "impeller/renderer/backend/vulkan/context_vk.h"
#include "impeller/renderer/backend/vulkan/swapchain/swapchain_transients_vk.h"
#include "impeller/renderer/backend/vulkan/texture_source_vk.h"
#include "impeller/renderer/surface.h"

namespace impeller {

class SurfaceVK final : public Surface {
 public:
  using SwapCallback = std::function<bool(void)>;

  /// @brief Wrap the swapchain image in a Surface, which provides the
  ///        additional configuration required for usage as on onscreen render
  ///        target by Impeller.
  ///
  ///        This creates the associated MSAA and depth+stencil texture.
  static std::unique_ptr<SurfaceVK> WrapSwapchainImage(
      const std::shared_ptr<SwapchainTransientsVK>& transients,
      const std::shared_ptr<TextureSourceVK>& swapchain_image,
      SwapCallback swap_callback);

  // |Surface|
  ~SurfaceVK() override;

 private:
  SwapCallback swap_callback_;

  SurfaceVK(const RenderTarget& target, SwapCallback swap_callback);

  // |Surface|
  bool Present() const override;

  SurfaceVK(const SurfaceVK&) = delete;

  SurfaceVK& operator=(const SurfaceVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_SURFACE_VK_H_
