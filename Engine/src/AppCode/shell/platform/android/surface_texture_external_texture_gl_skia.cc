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

#include "appcode/shell/platform/android/surface_texture_external_texture_gl_skia.h"

#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>

#include "appcode/third_party/skia/include/core/SkColorSpace.h"
#include "appcode/third_party/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "appcode/third_party/skia/include/gpu/ganesh/GrDirectContext.h"
#include "appcode/third_party/skia/include/gpu/ganesh/SkImageGanesh.h"
#include "appcode/third_party/skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "appcode/third_party/skia/include/gpu/ganesh/gl/GrGLTypes.h"

namespace appcode {

SurfaceTextureExternalTextureGLSkia::SurfaceTextureExternalTextureGLSkia(
    int64_t id,
    const fml::jni::ScopedJavaGlobalRef<jobject>& surface_texture,
    const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade)
    : SurfaceTextureExternalTexture(id, surface_texture, jni_facade) {}

SurfaceTextureExternalTextureGLSkia::~SurfaceTextureExternalTextureGLSkia() {
  if (texture_name_ != 0) {
    glDeleteTextures(1, &texture_name_);
  }
}

void SurfaceTextureExternalTextureGLSkia::ProcessFrame(PaintContext& context,
                                                       const SkRect& bounds) {
  if (state_ == AttachmentState::kUninitialized) {
    // Generate the texture handle.
    glGenTextures(1, &texture_name_);
    Attach(texture_name_);
  }
  FML_CHECK(state_ == AttachmentState::kAttached);

  // Updates the texture contents and transformation matrix.
  Update();

  GrGLTextureInfo textureInfo = {GL_TEXTURE_EXTERNAL_OES, texture_name_,
                                 GL_RGBA8_OES};
  auto backendTexture =
      GrBackendTextures::MakeGL(1, 1, skgpu::Mipmapped::kNo, textureInfo);
  dl_image_ = DlImage::Make(SkImages::BorrowTextureFrom(
      context.gr_context, backendTexture, kTopLeft_GrSurfaceOrigin,
      kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr));
}

void SurfaceTextureExternalTextureGLSkia::Detach() {
  SurfaceTextureExternalTexture::Detach();
  if (texture_name_ != 0) {
    glDeleteTextures(1, &texture_name_);
    texture_name_ = 0;
  }
}

}  // namespace appcode
