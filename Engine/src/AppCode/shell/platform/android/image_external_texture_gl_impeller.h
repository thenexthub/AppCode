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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_IMAGE_EXTERNAL_TEXTURE_GL_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_IMAGE_EXTERNAL_TEXTURE_GL_IMPELLER_H_

#include <memory>
#include <optional>

#include "appcode/fml/macros.h"
#include "appcode/impeller/renderer/backend/gles/context_gles.h"
#include "appcode/shell/platform/android/image_external_texture_gl.h"
#include "appcode/shell/platform/android/jni/platform_view_android_jni.h"

namespace appcode {

class ImageExternalTextureGLImpeller : public ImageExternalTextureGL {
 public:
  ImageExternalTextureGLImpeller(
      const std::shared_ptr<impeller::ContextGLES>& context,
      int64_t id,
      const fml::jni::ScopedJavaGlobalRef<jobject>&
          hardware_buffer_texture_entry,
      const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade,
      ImageExternalTexture::ImageLifecycle lifecycle);

 private:
  // |ImageExternalTexture|
  void Attach(PaintContext& context) override;

  // |ImageExternalTexture|
  void Detach() override;

  // |ImageExternalTextureGL|
  sk_sp<appcode::DlImage> CreateDlImage(
      PaintContext& context,
      const SkRect& bounds,
      std::optional<HardwareBufferKey> id,
      impeller::UniqueEGLImageKHR&& egl_image) override;

  const std::shared_ptr<impeller::ContextGLES> impeller_context_;

  FML_DISALLOW_COPY_AND_ASSIGN(ImageExternalTextureGLImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_IMAGE_EXTERNAL_TEXTURE_GL_IMPELLER_H_
