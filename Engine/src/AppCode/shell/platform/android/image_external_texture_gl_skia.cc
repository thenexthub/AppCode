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

#include "appcode/shell/platform/android/image_external_texture_gl_skia.h"

#include "appcode/third_party/skia/include/gpu/ganesh/SkImageGanesh.h"
#include "appcode/third_party/skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"

namespace appcode {

ImageExternalTextureGLSkia::ImageExternalTextureGLSkia(
    const std::shared_ptr<AndroidContextGLSkia>& context,
    int64_t id,
    const fml::jni::ScopedJavaGlobalRef<jobject>& image_texture_entry,
    const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade,
    ImageExternalTexture::ImageLifecycle lifecycle)
    : ImageExternalTextureGL(id, image_texture_entry, jni_facade, lifecycle) {}

void ImageExternalTextureGLSkia::Attach(PaintContext& context) {
  if (state_ == AttachmentState::kUninitialized) {
    // After this call state_ will be AttachmentState::kAttached and egl_image_
    // will have been created if we still have an Image associated with us.
    ImageExternalTextureGL::Attach(context);
  }
}

void ImageExternalTextureGLSkia::Detach() {
  ImageExternalTextureGL::Detach();
}

void ImageExternalTextureGLSkia::BindImageToTexture(
    const impeller::UniqueEGLImageKHR& image,
    GLuint tex) {
  if (!image.is_valid() || tex == 0) {
    return;
  }
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, tex);
  glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES,
                               static_cast<GLeglImageOES>(image.get().image));
}

sk_sp<appcode::DlImage> ImageExternalTextureGLSkia::CreateDlImage(
    PaintContext& context,
    const SkRect& bounds,
    std::optional<HardwareBufferKey> id,
    impeller::UniqueEGLImageKHR&& egl_image) {
  GLuint texture_name;
  glGenTextures(1, &texture_name);
  auto gl_texture = impeller::GLTexture{texture_name};
  impeller::UniqueGLTexture unique_texture;
  unique_texture.reset(gl_texture);

  BindImageToTexture(egl_image, unique_texture.get().texture_name);
  GrGLTextureInfo textureInfo = {
      GL_TEXTURE_EXTERNAL_OES, unique_texture.get().texture_name, GL_RGBA8_OES};
  auto backendTexture =
      GrBackendTextures::MakeGL(1, 1, skgpu::Mipmapped::kNo, textureInfo);

  gl_entries_[id.value_or(0)] = GlEntry{.egl_image = std::move(egl_image),
                                        .texture = std::move(unique_texture)};
  return DlImage::Make(SkImages::BorrowTextureFrom(
      context.gr_context, backendTexture, kTopLeft_GrSurfaceOrigin,
      kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr));
}

}  // namespace appcode
