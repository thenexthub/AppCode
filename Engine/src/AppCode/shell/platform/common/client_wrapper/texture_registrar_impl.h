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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_TEXTURE_REGISTRAR_IMPL_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_TEXTURE_REGISTRAR_IMPL_H_

#include "include/appcode/texture_registrar.h"

namespace appcode {

// Wrapper around a FlutterDesktopTextureRegistrarRef that implements the
// TextureRegistrar API.
class TextureRegistrarImpl : public TextureRegistrar {
 public:
  explicit TextureRegistrarImpl(
      FlutterDesktopTextureRegistrarRef texture_registrar_ref);
  virtual ~TextureRegistrarImpl();

  // Prevent copying.
  TextureRegistrarImpl(TextureRegistrarImpl const&) = delete;
  TextureRegistrarImpl& operator=(TextureRegistrarImpl const&) = delete;

  // |appcode::TextureRegistrar|
  int64_t RegisterTexture(TextureVariant* texture) override;

  // |appcode::TextureRegistrar|
  bool MarkTextureFrameAvailable(int64_t texture_id) override;

  // |appcode::TextureRegistrar|
  void UnregisterTexture(int64_t texture_id,
                         std::function<void()> callback) override;

  // |appcode::TextureRegistrar|
  bool UnregisterTexture(int64_t texture_id) override;

 private:
  // Handle for interacting with the C API.
  FlutterDesktopTextureRegistrarRef texture_registrar_ref_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_TEXTURE_REGISTRAR_IMPL_H_
