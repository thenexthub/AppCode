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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_GL_SKIA_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_GL_SKIA_H_

#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_counted.h"
#include "appcode/fml/memory/ref_ptr.h"
#include "appcode/shell/common/platform_view.h"
#include "appcode/shell/platform/android/android_environment_gl.h"
#include "appcode/shell/platform/android/context/android_context.h"
#include "appcode/shell/platform/android/surface/android_native_window.h"
#include "third_party/skia/include/core/SkSize.h"

namespace appcode {

class AndroidEGLSurface;

//------------------------------------------------------------------------------
/// The Android context is used by `AndroidSurfaceGL` to create and manage
/// EGL surfaces.
///
/// This context binds `EGLContext` to the current rendering thread and to the
/// draw and read `EGLSurface`s.
///
class AndroidContextGLSkia : public AndroidContext {
 public:
  AndroidContextGLSkia(fml::RefPtr<AndroidEnvironmentGL> environment,
                       const TaskRunners& taskRunners);

  ~AndroidContextGLSkia();

  //----------------------------------------------------------------------------
  /// @brief      Allocates an new EGL window surface that is used for on-screen
  ///             pixels.
  ///
  /// @return     The window surface.
  ///
  std::unique_ptr<AndroidEGLSurface> CreateOnscreenSurface(
      const fml::RefPtr<AndroidNativeWindow>& window) const;

  //----------------------------------------------------------------------------
  /// @brief      Allocates an 1x1 pbuffer surface that is used for making the
  ///             offscreen current for texture uploads.
  ///
  /// @return     The pbuffer surface.
  ///
  std::unique_ptr<AndroidEGLSurface> CreateOffscreenSurface() const;

  //----------------------------------------------------------------------------
  /// @brief      Allocates an 1x1 pbuffer surface that is used for making the
  ///             onscreen context current for snapshotting.
  ///
  /// @return     The pbuffer surface.
  ///
  std::unique_ptr<AndroidEGLSurface> CreatePbufferSurface() const;

  //----------------------------------------------------------------------------
  /// @return     The Android environment that contains a reference to the
  /// display.
  ///
  fml::RefPtr<AndroidEnvironmentGL> Environment() const;

  //----------------------------------------------------------------------------
  /// @return     Whether the current context is valid. That is, if the EGL
  ///             contexts were successfully created.
  ///
  bool IsValid() const override;

  //----------------------------------------------------------------------------
  /// @return     Whether the current context was successfully clear.
  ///
  bool ClearCurrent() const;

  //----------------------------------------------------------------------------
  /// @brief      Returns the EGLContext.
  ///
  /// @return     EGLContext.
  ///
  EGLContext GetEGLContext() const;

  //----------------------------------------------------------------------------
  /// @brief      Returns the EGLDisplay.
  ///
  /// @return     EGLDisplay.
  ///
  EGLDisplay GetEGLDisplay() const;

  //----------------------------------------------------------------------------
  /// @brief      Create a new EGLContext using the same EGLConfig.
  ///
  /// @return     The EGLContext.
  ///
  EGLContext CreateNewContext() const;

  //----------------------------------------------------------------------------
  /// @brief      The EGLConfig for this context.
  ///
  EGLConfig Config() const { return config_; }

 private:
  fml::RefPtr<AndroidEnvironmentGL> environment_;
  EGLConfig config_;
  EGLContext context_;
  EGLContext resource_context_;
  bool valid_ = false;
  TaskRunners task_runners_;

  FML_DISALLOW_COPY_AND_ASSIGN(AndroidContextGLSkia);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_GL_SKIA_H_
