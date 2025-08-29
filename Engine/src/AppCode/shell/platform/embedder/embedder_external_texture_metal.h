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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_EXTERNAL_TEXTURE_METAL_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_EXTERNAL_TEXTURE_METAL_H_

#include "appcode/common/graphics/texture.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "impeller/display_list/aiks_context.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkSize.h"

namespace appcode {

class EmbedderExternalTextureMetal : public appcode::Texture {
 public:
  using ExternalTextureCallback = std::function<
      std::unique_ptr<FlutterMetalExternalTexture>(int64_t, size_t, size_t)>;

  EmbedderExternalTextureMetal(int64_t texture_identifier,
                               const ExternalTextureCallback& callback);

  ~EmbedderExternalTextureMetal();

 private:
  const ExternalTextureCallback& external_texture_callback_;
  sk_sp<DlImage> last_image_;

  sk_sp<DlImage> ResolveTexture(int64_t texture_id,
                                GrDirectContext* context,
                                impeller::AiksContext* aiks_context,
                                const SkISize& size);

  // |appcode::Texture|
  void Paint(PaintContext& context,
             const DlRect& bounds,
             bool freeze,
             const DlImageSampling sampling) override;

  // |appcode::Texture|
  void OnGrContextCreated() override;

  // |appcode::Texture|
  void OnGrContextDestroyed() override;

  // |appcode::Texture|
  void MarkNewFrameAvailable() override;

  // |appcode::Texture|
  void OnTextureUnregistered() override;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderExternalTextureMetal);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_EXTERNAL_TEXTURE_METAL_H_
