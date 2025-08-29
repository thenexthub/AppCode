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

#include "impeller/core/texture.h"

#include "impeller/base/validation.h"

namespace impeller {

Texture::Texture(TextureDescriptor desc) : desc_(desc) {}

Texture::~Texture() = default;

bool Texture::SetContents(const uint8_t* contents,
                          size_t length,
                          size_t slice,
                          bool is_opaque) {
  if (!IsSliceValid(slice)) {
    VALIDATION_LOG << "Invalid slice for texture.";
    return false;
  }
  if (!OnSetContents(contents, length, slice)) {
    return false;
  }
  coordinate_system_ = TextureCoordinateSystem::kUploadFromHost;
  is_opaque_ = is_opaque;
  return true;
}

bool Texture::SetContents(std::shared_ptr<const fml::Mapping> mapping,
                          size_t slice,
                          bool is_opaque) {
  if (!IsSliceValid(slice)) {
    VALIDATION_LOG << "Invalid slice for texture.";
    return false;
  }
  if (!mapping) {
    return false;
  }
  if (!OnSetContents(std::move(mapping), slice)) {
    return false;
  }
  coordinate_system_ = TextureCoordinateSystem::kUploadFromHost;
  is_opaque_ = is_opaque;
  return true;
}

bool Texture::IsOpaque() const {
  return is_opaque_;
}

size_t Texture::GetMipCount() const {
  return GetTextureDescriptor().mip_count;
}

const TextureDescriptor& Texture::GetTextureDescriptor() const {
  return desc_;
}

bool Texture::IsSliceValid(size_t slice) const {
  switch (desc_.type) {
    case TextureType::kTexture2D:
    case TextureType::kTexture2DMultisample:
    case TextureType::kTextureExternalOES:
      return slice == 0;
    case TextureType::kTextureCube:
      return slice <= 5;
  }
  FML_UNREACHABLE();
}

void Texture::SetCoordinateSystem(TextureCoordinateSystem coordinate_system) {
  coordinate_system_ = coordinate_system;
}

TextureCoordinateSystem Texture::GetCoordinateSystem() const {
  return coordinate_system_;
}

Scalar Texture::GetYCoordScale() const {
  return 1.0;
}

bool Texture::NeedsMipmapGeneration() const {
  return !mipmap_generated_ && desc_.mip_count > 1;
}

}  // namespace impeller
