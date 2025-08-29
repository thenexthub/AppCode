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

#include "impeller/toolkit/egl/surface.h"

namespace impeller {
namespace egl {

Surface::Surface(EGLDisplay display, EGLSurface surface)
    : display_(display), surface_(surface) {}

Surface::~Surface() {
  if (surface_ != EGL_NO_SURFACE) {
    if (::eglDestroySurface(display_, surface_) != EGL_TRUE) {
      IMPELLER_LOG_EGL_ERROR;
    }
  }
}

const EGLSurface& Surface::GetHandle() const {
  return surface_;
}

bool Surface::IsValid() const {
  return surface_ != EGL_NO_SURFACE;
}

bool Surface::Present() const {
  const auto result = ::eglSwapBuffers(display_, surface_) == EGL_TRUE;
  if (!result) {
    IMPELLER_LOG_EGL_ERROR;
  }
  return result;
}

}  // namespace egl
}  // namespace impeller
