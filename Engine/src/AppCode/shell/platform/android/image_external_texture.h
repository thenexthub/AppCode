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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_IMAGE_EXTERNAL_TEXTURE_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_IMAGE_EXTERNAL_TEXTURE_H_

#include "appcode/common/graphics/texture.h"
#include "appcode/fml/logging.h"
#include "appcode/shell/platform/android/image_lru.h"
#include "appcode/shell/platform/android/jni/platform_view_android_jni.h"
#include "appcode/shell/platform/android/platform_view_android_jni_impl.h"

#include <android/hardware_buffer.h>
#include <android/hardware_buffer_jni.h>

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      External texture peered to a sequence of
///             android.hardware.HardwareBuffers.
///
///             Android Hardware Buffers are available on newer versions of
///             Android (API 29 and above).
///
///             This is an abstract base class and graphics packages provide
///             concrete implementations of this class that bind hardware
///             buffers to their own package-specific implementations of
///             textures (SkImages, impeller::Texture, etc...).
///
///             Android Hardware Buffers allow binding to both OpenGL and Vulkan
///             client-rendering APIs in a zero copy manner. Because of this
///             graphics packages that support OpenGL and Vulkan can have
///             multiple subclasses for each supported client-rendering API.
///
class ImageExternalTexture : public appcode::Texture {
 public:
  /// Whether the last image should be reset when the context is destroyed.
  enum class ImageLifecycle { kReset, kKeepAlive };

  explicit ImageExternalTexture(
      int64_t id,
      const fml::jni::ScopedJavaGlobalRef<jobject>& image_texture_entry,
      const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade,
      ImageLifecycle lifecycle);

  // |appcode::Texture|
  virtual ~ImageExternalTexture();

 protected:
  //----------------------------------------------------------------------------
  /// @brief      Subclasses override this method to bind the latest
  ///             representation of the hardware buffer wrapped by this texture
  ///             instance to their own graphics package specific texture
  ///             classes (SkImage, impeller::Texture, etc...).
  ///
  ///             After a call to this method, attachment state of this instance
  ///             must be kAttached. That is the responsibility of the subclass.
  ///
  /// @param      context  The context
  /// @param[in]  bounds   The bounds
  ///
  virtual void ProcessFrame(PaintContext& context, const SkRect& bounds) = 0;

  virtual void Attach(PaintContext& context) = 0;

  virtual void Detach() = 0;

  JavaLocalRef AcquireLatestImage();

  void CloseImage(const fml::jni::JavaRef<jobject>& image);

  JavaLocalRef HardwareBufferFor(const fml::jni::JavaRef<jobject>& image);

  void CloseHardwareBuffer(const fml::jni::JavaRef<jobject>& hardware_buffer);

  AHardwareBuffer* AHardwareBufferFor(
      const fml::jni::JavaRef<jobject>& hardware_buffer);

  fml::jni::ScopedJavaGlobalRef<jobject> image_texture_entry_;
  std::shared_ptr<PlatformViewAndroidJNI> jni_facade_;

  enum class AttachmentState { kUninitialized, kAttached, kDetached };
  AttachmentState state_ = AttachmentState::kUninitialized;
  sk_sp<appcode::DlImage> dl_image_;
  ImageLRU image_lru_ = ImageLRU();

 private:
  // |appcode::Texture|.
  void Paint(PaintContext& context,
             const DlRect& bounds,
             bool freeze,
             const DlImageSampling sampling) override;

  // |appcode::Texture|.
  void MarkNewFrameAvailable() override;

  // |appcode::Texture|
  void OnTextureUnregistered() override;

  // |appcode::ContextListener|
  void OnGrContextCreated() override;

  // |appcode::ContextListener|
  void OnGrContextDestroyed() override;

  const ImageLifecycle texture_lifecycle_;
  FML_DISALLOW_COPY_AND_ASSIGN(ImageExternalTexture);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_IMAGE_EXTERNAL_TEXTURE_H_
