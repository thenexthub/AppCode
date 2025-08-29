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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_BLIT_PASS_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_BLIT_PASS_VK_H_

#include "appcode/impeller/base/config.h"
#include "impeller/geometry/rect.h"
#include "impeller/renderer/backend/vulkan/workarounds_vk.h"
#include "impeller/renderer/blit_pass.h"

namespace impeller {

class CommandEncoderVK;
class CommandBufferVK;

class BlitPassVK final : public BlitPass {
 public:
  // |BlitPass|
  ~BlitPassVK() override;

 private:
  friend class CommandBufferVK;

  std::shared_ptr<CommandBufferVK> command_buffer_;
  const WorkaroundsVK workarounds_;

  explicit BlitPassVK(std::shared_ptr<CommandBufferVK> command_buffer,
                      const WorkaroundsVK& workarounds);

  // |BlitPass|
  bool IsValid() const override;

  // |BlitPass|
  void OnSetLabel(std::string_view label) override;

  // |BlitPass|
  bool EncodeCommands() const override;

  // |BlitPass|
  bool ResizeTexture(const std::shared_ptr<Texture>& source,
                     const std::shared_ptr<Texture>& destination) override;

  // |BlitPass|
  bool ConvertTextureToShaderRead(
      const std::shared_ptr<Texture>& texture) override;

  // |BlitPass|
  bool OnCopyTextureToTextureCommand(std::shared_ptr<Texture> source,
                                     std::shared_ptr<Texture> destination,
                                     IRect source_region,
                                     IPoint destination_origin,
                                     std::string_view label) override;

  // |BlitPass|
  bool OnCopyTextureToBufferCommand(std::shared_ptr<Texture> source,
                                    std::shared_ptr<DeviceBuffer> destination,
                                    IRect source_region,
                                    size_t destination_offset,
                                    std::string_view label) override;

  // |BlitPass|
  bool OnCopyBufferToTextureCommand(BufferView source,
                                    std::shared_ptr<Texture> destination,
                                    IRect destination_region,
                                    std::string_view label,
                                    uint32_t mip_level,
                                    uint32_t slice,
                                    bool convert_to_read) override;
  // |BlitPass|
  bool OnGenerateMipmapCommand(std::shared_ptr<Texture> texture,
                               std::string_view label) override;

  BlitPassVK(const BlitPassVK&) = delete;

  BlitPassVK& operator=(const BlitPassVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_BLIT_PASS_VK_H_
