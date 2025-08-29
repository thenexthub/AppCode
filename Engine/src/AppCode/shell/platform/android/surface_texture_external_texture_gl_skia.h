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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_TEXTURE_EXTERNAL_TEXTURE_GL_SKIA_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_TEXTURE_EXTERNAL_TEXTURE_GL_SKIA_H_

#include <memory>

#include "appcode/shell/platform/android/surface_texture_external_texture.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      Binds the surface texture to a Skia SkImage.
///
class SurfaceTextureExternalTextureGLSkia
    : public SurfaceTextureExternalTexture {
 public:
  SurfaceTextureExternalTextureGLSkia(
      int64_t id,
      const fml::jni::ScopedJavaGlobalRef<jobject>& surface_texture,
      const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade);

  // |SurfaceTextureExternalTexture|
  ~SurfaceTextureExternalTextureGLSkia() override;

 private:
  // |SurfaceTextureExternalTexture|
  virtual void ProcessFrame(PaintContext& context,
                            const SkRect& bounds) override;

  // |SurfaceTextureExternalTexture|
  virtual void Detach() override;

  GLuint texture_name_ = 0;

  FML_DISALLOW_COPY_AND_ASSIGN(SurfaceTextureExternalTextureGLSkia);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_TEXTURE_EXTERNAL_TEXTURE_GL_SKIA_H_
