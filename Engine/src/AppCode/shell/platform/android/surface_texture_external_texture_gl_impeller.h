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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_TEXTURE_EXTERNAL_TEXTURE_GL_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_TEXTURE_EXTERNAL_TEXTURE_GL_IMPELLER_H_

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/impeller/renderer/backend/gles/context_gles.h"
#include "appcode/impeller/renderer/backend/gles/texture_gles.h"
#include "appcode/shell/platform/android/surface_texture_external_texture.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      Binds the surface texture to an Impeller texture.
///
class SurfaceTextureExternalTextureGLImpeller
    : public SurfaceTextureExternalTexture {
 public:
  SurfaceTextureExternalTextureGLImpeller(
      const std::shared_ptr<impeller::ContextGLES>& context,
      int64_t id,
      const fml::jni::ScopedJavaGlobalRef<jobject>& surface_texture,
      const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade);

  // |SurfaceTextureExternalTexture|
  ~SurfaceTextureExternalTextureGLImpeller() override;

 private:
  // |SurfaceTextureExternalTexture|
  virtual void ProcessFrame(PaintContext& context,
                            const SkRect& bounds) override;

  // |SurfaceTextureExternalTexture|
  virtual void Detach() override;

  const std::shared_ptr<impeller::ContextGLES> impeller_context_;
  std::shared_ptr<impeller::TextureGLES> texture_;

  FML_DISALLOW_COPY_AND_ASSIGN(SurfaceTextureExternalTextureGLImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_TEXTURE_EXTERNAL_TEXTURE_GL_IMPELLER_H_
