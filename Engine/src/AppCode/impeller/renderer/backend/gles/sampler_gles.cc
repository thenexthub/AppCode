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

#include "impeller/renderer/backend/gles/sampler_gles.h"

#include "impeller/base/validation.h"
#include "impeller/core/formats.h"
#include "impeller/core/sampler_descriptor.h"
#include "impeller/renderer/backend/gles/formats_gles.h"
#include "impeller/renderer/backend/gles/proc_table_gles.h"
#include "impeller/renderer/backend/gles/texture_gles.h"

namespace impeller {

SamplerGLES::SamplerGLES(const SamplerDescriptor& desc) : Sampler(desc) {}

SamplerGLES::~SamplerGLES() = default;

static GLint ToParam(MinMagFilter minmag_filter) {
  switch (minmag_filter) {
    case MinMagFilter::kNearest:
      return GL_NEAREST;
    case MinMagFilter::kLinear:
      return GL_LINEAR;
  }
  FML_UNREACHABLE();
}

static GLint ToParam(MinMagFilter minmag_filter, MipFilter mip_filter) {
  switch (mip_filter) {
    case MipFilter::kBase:
      return ToParam(minmag_filter);
    case MipFilter::kNearest:
      switch (minmag_filter) {
        case MinMagFilter::kNearest:
          return GL_NEAREST_MIPMAP_NEAREST;
        case MinMagFilter::kLinear:
          return GL_LINEAR_MIPMAP_NEAREST;
      }
    case MipFilter::kLinear:
      switch (minmag_filter) {
        case MinMagFilter::kNearest:
          return GL_NEAREST_MIPMAP_LINEAR;
        case MinMagFilter::kLinear:
          return GL_LINEAR_MIPMAP_LINEAR;
      }
  }
  FML_UNREACHABLE();
}

static GLint ToAddressMode(SamplerAddressMode mode,
                           bool supports_decal_sampler_address_mode) {
  switch (mode) {
    case SamplerAddressMode::kClampToEdge:
      return GL_CLAMP_TO_EDGE;
    case SamplerAddressMode::kRepeat:
      return GL_REPEAT;
    case SamplerAddressMode::kMirror:
      return GL_MIRRORED_REPEAT;
    case SamplerAddressMode::kDecal:
      if (supports_decal_sampler_address_mode) {
        return IMPELLER_GL_CLAMP_TO_BORDER;
      } else {
        return GL_CLAMP_TO_EDGE;
      }
  }
  FML_UNREACHABLE();
}

bool SamplerGLES::ConfigureBoundTexture(const TextureGLES& texture,
                                        const ProcTableGLES& gl) const {
  if (texture.NeedsMipmapGeneration()) {
    VALIDATION_LOG
        << "Texture mip count is > 1, but the mipmap has not been generated. "
           "Texture can not be sampled safely.";
    return false;
  }

  auto target = ToTextureTarget(texture.GetTextureDescriptor().type);

  if (!target.has_value()) {
    return false;
  }
  const SamplerDescriptor& desc = GetDescriptor();

  GLint mag_filter = ToParam(desc.mag_filter);

  // If the texture doesn't have mipmaps, we can't use mip filtering.
  GLint min_filter;
  if (texture.GetTextureDescriptor().mip_count > 1) {
    min_filter = ToParam(desc.min_filter, desc.mip_filter);
  } else {
    min_filter = ToParam(desc.min_filter);
  }

  gl.TexParameteri(*target, GL_TEXTURE_MIN_FILTER, min_filter);
  gl.TexParameteri(*target, GL_TEXTURE_MAG_FILTER, mag_filter);

  const auto supports_decal_mode =
      gl.GetCapabilities()->SupportsDecalSamplerAddressMode();

  const auto wrap_s =
      ToAddressMode(desc.width_address_mode, supports_decal_mode);
  const auto wrap_t =
      ToAddressMode(desc.height_address_mode, supports_decal_mode);

  gl.TexParameteri(*target, GL_TEXTURE_WRAP_S, wrap_s);
  gl.TexParameteri(*target, GL_TEXTURE_WRAP_T, wrap_t);

  if (wrap_s == IMPELLER_GL_CLAMP_TO_BORDER ||
      wrap_t == IMPELLER_GL_CLAMP_TO_BORDER) {
    // Transparent black.
    const GLfloat border_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    gl.TexParameterfv(*target, IMPELLER_GL_TEXTURE_BORDER_COLOR, border_color);
  }

  return true;
}

}  // namespace impeller
