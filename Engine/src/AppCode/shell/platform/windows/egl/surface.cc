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

#include "appcode/shell/platform/windows/egl/surface.h"

#include "appcode/shell/platform/windows/egl/egl.h"

namespace appcode {
namespace egl {

Surface::Surface(EGLDisplay display, EGLContext context, EGLSurface surface)
    : display_(display), context_(context), surface_(surface) {}

Surface::~Surface() {
  Destroy();
}

bool Surface::IsValid() const {
  return is_valid_;
}

bool Surface::Destroy() {
  if (surface_ != EGL_NO_SURFACE) {
    // Ensure the surface is not current before destroying it.
    if (::eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                         EGL_NO_CONTEXT) != EGL_TRUE) {
      WINDOWS_LOG_EGL_ERROR;
      return false;
    }

    if (::eglDestroySurface(display_, surface_) != EGL_TRUE) {
      WINDOWS_LOG_EGL_ERROR;
      return false;
    }
  }

  is_valid_ = false;
  surface_ = EGL_NO_SURFACE;
  return true;
}

bool Surface::IsCurrent() const {
  return display_ == ::eglGetCurrentDisplay() &&
         surface_ == ::eglGetCurrentSurface(EGL_DRAW) &&
         surface_ == ::eglGetCurrentSurface(EGL_READ) &&
         context_ == ::eglGetCurrentContext();
}

bool Surface::MakeCurrent() const {
  if (::eglMakeCurrent(display_, surface_, surface_, context_) != EGL_TRUE) {
    WINDOWS_LOG_EGL_ERROR;
    return false;
  }

  return true;
}

bool Surface::SwapBuffers() const {
  if (::eglSwapBuffers(display_, surface_) != EGL_TRUE) {
    WINDOWS_LOG_EGL_ERROR;
    return false;
  }

  return true;
}

const EGLSurface& Surface::GetHandle() const {
  return surface_;
}

}  // namespace egl
}  // namespace appcode
