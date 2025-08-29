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

#ifndef APPCODE_IMPELLER_CORE_TEXTURE_H_
#define APPCODE_IMPELLER_CORE_TEXTURE_H_

#include <string_view>

#include "appcode/fml/mapping.h"
#include "impeller/core/formats.h"
#include "impeller/core/texture_descriptor.h"
#include "impeller/geometry/size.h"

namespace impeller {

class Texture {
 public:
  virtual ~Texture();

  /// @brief Label this resource for inspection in GPU debugging tools.
  ///
  /// This functionality may be disabled in release builds.
  virtual void SetLabel(std::string_view label) = 0;

  /// @brief Label this resource for inspection in GPU debugging tools, with
  ///        label and trailing will be concatenated together.
  ///
  /// This functionality may be disabled in release builds.
  virtual void SetLabel(std::string_view label, std::string_view trailing) = 0;

  // Deprecated: use BlitPass::AddCopy instead.
  [[nodiscard]] bool SetContents(const uint8_t* contents,
                                 size_t length,
                                 size_t slice = 0,
                                 bool is_opaque = false);

  // Deprecated: use BlitPass::AddCopy instead.
  [[nodiscard]] bool SetContents(std::shared_ptr<const fml::Mapping> mapping,
                                 size_t slice = 0,
                                 bool is_opaque = false);

  virtual bool IsValid() const = 0;

  virtual ISize GetSize() const = 0;

  bool IsOpaque() const;

  size_t GetMipCount() const;

  const TextureDescriptor& GetTextureDescriptor() const;

  /// Update the coordinate system used by the texture.
  ///
  /// The setting is used to conditionally invert the coordinates to
  /// account for the different origin of GLES textures.
  void SetCoordinateSystem(TextureCoordinateSystem coordinate_system);

  TextureCoordinateSystem GetCoordinateSystem() const;

  virtual Scalar GetYCoordScale() const;

  /// Returns true if mipmaps have never been generated.
  /// The contents of the mipmap may be out of date if the root texture has been
  /// modified and the mipmaps hasn't been regenerated.
  bool NeedsMipmapGeneration() const;

 protected:
  explicit Texture(TextureDescriptor desc);

  [[nodiscard]] virtual bool OnSetContents(const uint8_t* contents,
                                           size_t length,
                                           size_t slice) = 0;

  [[nodiscard]] virtual bool OnSetContents(
      std::shared_ptr<const fml::Mapping> mapping,
      size_t slice) = 0;

  bool mipmap_generated_ = false;

 private:
  TextureCoordinateSystem coordinate_system_ =
      TextureCoordinateSystem::kRenderToTexture;
  const TextureDescriptor desc_;
  bool is_opaque_ = false;

  bool IsSliceValid(size_t slice) const;

  Texture(const Texture&) = delete;

  Texture& operator=(const Texture&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_TEXTURE_H_
