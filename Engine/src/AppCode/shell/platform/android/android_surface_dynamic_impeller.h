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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_SURFACE_DYNAMIC_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_SURFACE_DYNAMIC_IMPELLER_H_

#include "appcode/fml/concurrent_message_loop.h"
#include "appcode/fml/macros.h"
#include "appcode/impeller/display_list/aiks_context.h"
#include "appcode/impeller/renderer/backend/vulkan/surface_context_vk.h"
#include "appcode/shell/platform/android/android_context_vk_impeller.h"
#include "appcode/shell/platform/android/surface/android_native_window.h"
#include "appcode/shell/platform/android/surface/android_surface.h"
#include "shell/gpu/gpu_surface_vulkan_impeller.h"
#include "shell/platform/android/android_context_dynamic_impeller.h"
#include "shell/platform/android/android_surface_gl_impeller.h"
#include "shell/platform/android/android_surface_vk_impeller.h"

namespace appcode {

/// @brief An Impeller Android surface class that dynamically creates either an
/// [AndroidSurfaceVKImpeller] or an [AndroidSurfaceGLImpeller].
///
/// The backing surface is created the first time [CreateGPUSurface] is called.
class AndroidSurfaceDynamicImpeller : public AndroidSurface {
 public:
  explicit AndroidSurfaceDynamicImpeller(
      std::shared_ptr<AndroidContextDynamicImpeller>& android_context);

  ~AndroidSurfaceDynamicImpeller() override;

  // |AndroidSurface|
  bool IsValid() const override;

  // |AndroidSurface|
  std::unique_ptr<Surface> CreateGPUSurface(
      GrDirectContext* gr_context) override;

  // |AndroidSurface|
  void TeardownOnScreenContext() override;

  // |AndroidSurface|
  bool OnScreenSurfaceResize(const SkISize& size) override;

  // |AndroidSurface|
  bool ResourceContextMakeCurrent() override;

  // |AndroidSurface|
  bool ResourceContextClearCurrent() override;

  // |AndroidSurface|
  std::shared_ptr<impeller::Context> GetImpellerContext() override;

  // |AndroidSurface|
  bool SetNativeWindow(
      fml::RefPtr<AndroidNativeWindow> window,
      const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade) override;

  // |AndroidSurface|
  std::unique_ptr<Surface> CreateSnapshotSurface() override;

  // |AndroidSurface|
  void SetupImpellerSurface() override;

 private:
  std::shared_ptr<AndroidContextDynamicImpeller> android_context_;
  std::unique_ptr<AndroidSurfaceVKImpeller> vulkan_surface_;
  std::unique_ptr<AndroidSurfaceGLImpeller> gl_surface_;
  fml::RefPtr<AndroidNativeWindow> window_;
  std::shared_ptr<PlatformViewAndroidJNI> jni_facade_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(AndroidSurfaceDynamicImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_SURFACE_DYNAMIC_IMPELLER_H_
