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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_ANDROID_SURFACE_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_ANDROID_SURFACE_H_

#include <memory>
#include "appcode/flow/embedded_views.h"
#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/platform/android/context/android_context.h"
#include "appcode/shell/platform/android/jni/platform_view_android_jni.h"
#include "appcode/shell/platform/android/surface/android_native_window.h"
#include "third_party/skia/include/core/SkSize.h"

namespace impeller {
class Context;
}  // namespace impeller

namespace appcode {

class AndroidExternalViewEmbedder;

class AndroidSurface {
 public:
  virtual ~AndroidSurface();

  virtual bool IsValid() const = 0;

  virtual void TeardownOnScreenContext() = 0;

  virtual std::unique_ptr<Surface> CreateGPUSurface(
      GrDirectContext* gr_context = nullptr) = 0;

  virtual bool OnScreenSurfaceResize(const SkISize& size) = 0;

  virtual bool ResourceContextMakeCurrent() = 0;

  virtual bool ResourceContextClearCurrent() = 0;

  virtual bool SetNativeWindow(
      fml::RefPtr<AndroidNativeWindow> window,
      const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade) = 0;

  virtual std::unique_ptr<Surface> CreateSnapshotSurface();

  virtual std::shared_ptr<impeller::Context> GetImpellerContext();

  virtual void SetupImpellerSurface();

 protected:
  AndroidSurface();
};

class AndroidSurfaceFactory {
 public:
  AndroidSurfaceFactory() = default;

  virtual ~AndroidSurfaceFactory() = default;

  virtual std::unique_ptr<AndroidSurface> CreateSurface() = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_ANDROID_SURFACE_H_
