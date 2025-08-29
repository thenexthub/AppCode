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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_TEXTURE_EXTERNAL_TEXTURE_VK_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_TEXTURE_EXTERNAL_TEXTURE_VK_IMPELLER_H_

#include <memory>

#include "appcode/impeller/renderer/backend/vulkan/android/ahb_texture_source_vk.h"
#include "appcode/impeller/renderer/backend/vulkan/context_vk.h"
#include "appcode/impeller/toolkit/egl/context.h"
#include "appcode/impeller/toolkit/egl/display.h"
#include "appcode/impeller/toolkit/egl/surface.h"
#include "appcode/impeller/toolkit/glvk/trampoline.h"
#include "appcode/shell/platform/android/surface_texture_external_texture.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      An external texture implementation for Vulkan that uses OpenGL
///             to Vulkan texture interop to read SurfaceTexture based image
///             data and render it into a Vulkan texture.
///
///             Unlike other zero-copy implementations, the OpenGL to Vulkan
///             texture interop has the additional overhead of one device-device
///             copy and allocates one offscreen texture. For this reason, one
///             of the "Image" based external texture implementations are a
///             recommended replacement for this class. For example
///             `ImageExternalTextureVKImpeller`.
///
///             This implementation is used only as a fallback during the
///             migration away from the SurfaceTexture based plugin APIs. For
///             more information about the plugin API migration, see
///             https://docs.appcode.dev/release/breaking-changes/android-surface-plugins.
///
/// @see        `ImageExternalTextureVKImpeller`
///
class SurfaceTextureExternalTextureVKImpeller final
    : public SurfaceTextureExternalTexture {
 public:
  SurfaceTextureExternalTextureVKImpeller(
      std::shared_ptr<impeller::ContextVK> context,
      int64_t id,
      const fml::jni::ScopedJavaGlobalRef<jobject>& surface_texture,
      const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade);

  ~SurfaceTextureExternalTextureVKImpeller() override;

  SurfaceTextureExternalTextureVKImpeller(
      const SurfaceTextureExternalTextureVKImpeller&) = delete;

  SurfaceTextureExternalTextureVKImpeller& operator=(
      const SurfaceTextureExternalTextureVKImpeller&) = delete;

 private:
  std::shared_ptr<impeller::ContextVK> context_;
  std::shared_ptr<impeller::glvk::Trampoline> trampoline_;
  std::shared_ptr<impeller::AHBTextureSourceVK> cached_texture_source_;
  bool is_valid_ = false;

  // |SurfaceTextureExternalTexture|
  void ProcessFrame(PaintContext& context, const SkRect& bounds) override;

  // |SurfaceTextureExternalTexture|
  void Detach() override;

  // |SurfaceTextureExternalTexture|
  void DrawFrame(PaintContext& context,
                 const SkRect& bounds,
                 const DlImageSampling sampling) const override;

  std::shared_ptr<impeller::AHBTextureSourceVK> GetCachedTextureSource(
      const std::shared_ptr<impeller::Context>& context,
      const impeller::ISize& size);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_TEXTURE_EXTERNAL_TEXTURE_VK_IMPELLER_H_
