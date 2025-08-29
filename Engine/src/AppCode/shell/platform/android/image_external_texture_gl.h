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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_IMAGE_EXTERNAL_TEXTURE_GL_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_IMAGE_EXTERNAL_TEXTURE_GL_H_

#include <memory>
#include <optional>
#include <unordered_map>

#include "appcode/impeller/toolkit/egl/image.h"
#include "appcode/impeller/toolkit/gles/texture.h"
#include "appcode/shell/platform/android/image_external_texture.h"

namespace appcode {

class ImageExternalTextureGL : public ImageExternalTexture {
 public:
  ImageExternalTextureGL(
      int64_t id,
      const fml::jni::ScopedJavaGlobalRef<jobject>& image_textury_entry,
      const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade,
      ImageExternalTexture::ImageLifecycle lifecycle);

 protected:
  // |ImageExternalTexture|
  void Attach(PaintContext& context) override;

  // |ImageExternalTexture|
  void Detach() override;

  // |ImageExternalTexture|
  void ProcessFrame(PaintContext& context, const SkRect& bounds) override;

  virtual sk_sp<appcode::DlImage> CreateDlImage(
      PaintContext& context,
      const SkRect& bounds,
      std::optional<HardwareBufferKey> id,
      impeller::UniqueEGLImageKHR&& egl_image) = 0;

  void UpdateImage(JavaLocalRef& hardware_buffer,
                   const SkRect& bounds,
                   PaintContext& context);

  impeller::UniqueEGLImageKHR CreateEGLImage(AHardwareBuffer* buffer);

  struct GlEntry {
    impeller::UniqueEGLImageKHR egl_image;
    impeller::UniqueGLTexture texture;
  };

  // Each GL entry is keyed off of the currently active
  // hardware buffers and evicted when the hardware buffer
  // is removed from the LRU cache.
  std::unordered_map<HardwareBufferKey, GlEntry> gl_entries_;

  FML_DISALLOW_COPY_AND_ASSIGN(ImageExternalTextureGL);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_IMAGE_EXTERNAL_TEXTURE_GL_H_
