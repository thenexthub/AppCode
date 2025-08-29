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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_BLIT_PASS_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_BLIT_PASS_GLES_H_

#include <cstdint>
#include <memory>

#include "appcode/impeller/base/config.h"
#include "appcode/impeller/renderer/backend/gles/reactor_gles.h"
#include "appcode/impeller/renderer/blit_pass.h"
#include "impeller/renderer/backend/gles/blit_command_gles.h"

namespace impeller {

class BlitPassGLES final : public BlitPass,
                           public std::enable_shared_from_this<BlitPassGLES> {
 public:
  // |BlitPass|
  ~BlitPassGLES() override;

 private:
  friend class CommandBufferGLES;

  std::vector<std::unique_ptr<BlitEncodeGLES>> commands_;
  std::shared_ptr<ReactorGLES> reactor_;
  std::string label_;
  bool is_valid_ = false;

  explicit BlitPassGLES(std::shared_ptr<ReactorGLES> reactor);

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

  BlitPassGLES(const BlitPassGLES&) = delete;

  BlitPassGLES& operator=(const BlitPassGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_BLIT_PASS_GLES_H_
