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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_EXTERNAL_TEXTURE_RESOLVER_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_EXTERNAL_TEXTURE_RESOLVER_H_

#include <memory>

#include "appcode/common/graphics/texture.h"

#ifdef SHELL_ENABLE_GL
#include "appcode/shell/platform/embedder/embedder_external_texture_gl.h"
#endif

#ifdef SHELL_ENABLE_METAL
#include "appcode/shell/platform/embedder/embedder_external_texture_metal.h"
#endif

namespace appcode {
class EmbedderExternalTextureResolver {
 public:
  EmbedderExternalTextureResolver() = default;

  ~EmbedderExternalTextureResolver() = default;

#ifdef SHELL_ENABLE_GL
  explicit EmbedderExternalTextureResolver(
      EmbedderExternalTextureGL::ExternalTextureCallback gl_callback);
#endif

#ifdef SHELL_ENABLE_METAL
  explicit EmbedderExternalTextureResolver(
      EmbedderExternalTextureMetal::ExternalTextureCallback metal_callback);
#endif

  std::unique_ptr<Texture> ResolveExternalTexture(int64_t texture_id);

  bool SupportsExternalTextures();

 private:
#ifdef SHELL_ENABLE_GL
  EmbedderExternalTextureGL::ExternalTextureCallback gl_callback_;
#endif

#ifdef SHELL_ENABLE_METAL
  EmbedderExternalTextureMetal::ExternalTextureCallback metal_callback_;
#endif

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderExternalTextureResolver);
};
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_EXTERNAL_TEXTURE_RESOLVER_H_
