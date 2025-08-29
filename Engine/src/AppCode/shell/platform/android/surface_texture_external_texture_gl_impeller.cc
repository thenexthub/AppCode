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

#include "appcode/shell/platform/android/surface_texture_external_texture_gl_impeller.h"

#include "appcode/impeller/display_list/dl_image_impeller.h"

namespace appcode {

SurfaceTextureExternalTextureGLImpeller::
    SurfaceTextureExternalTextureGLImpeller(
        const std::shared_ptr<impeller::ContextGLES>& context,
        int64_t id,
        const fml::jni::ScopedJavaGlobalRef<jobject>& surface_texture,
        const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade)
    : SurfaceTextureExternalTexture(id, surface_texture, jni_facade),
      impeller_context_(context) {}

SurfaceTextureExternalTextureGLImpeller::
    ~SurfaceTextureExternalTextureGLImpeller() = default;

void SurfaceTextureExternalTextureGLImpeller::ProcessFrame(
    PaintContext& context,
    const SkRect& bounds) {
  if (state_ == AttachmentState::kUninitialized) {
    // Generate the texture handle.
    impeller::TextureDescriptor desc;
    desc.type = impeller::TextureType::kTextureExternalOES;
    desc.storage_mode = impeller::StorageMode::kDevicePrivate;
    desc.format = impeller::PixelFormat::kR8G8B8A8UNormInt;
    desc.size = {1, 1};
    desc.mip_count = 1;
    texture_ = std::make_shared<impeller::TextureGLES>(
        impeller_context_->GetReactor(), desc);
    // The contents will be initialized later in the call to `Attach` instead of
    // by Impeller.
    texture_->MarkContentsInitialized();
    texture_->SetCoordinateSystem(
        impeller::TextureCoordinateSystem::kUploadFromHost);
    auto maybe_handle = texture_->GetGLHandle();
    if (!maybe_handle.has_value()) {
      FML_LOG(ERROR) << "Could not get GL handle from impeller::TextureGLES!";
      return;
    }
    Attach(maybe_handle.value());
  }
  FML_CHECK(state_ == AttachmentState::kAttached);

  // Updates the texture contents and transformation matrix.
  Update();

  dl_image_ = impeller::DlImageImpeller::Make(texture_);
}

void SurfaceTextureExternalTextureGLImpeller::Detach() {
  SurfaceTextureExternalTexture::Detach();
  texture_.reset();
}

}  // namespace appcode
