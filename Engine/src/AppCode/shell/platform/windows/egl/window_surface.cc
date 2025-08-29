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

#include "appcode/shell/platform/windows/egl/window_surface.h"

#include "appcode/fml/logging.h"
#include "appcode/shell/platform/windows/egl/egl.h"

namespace appcode {
namespace egl {

WindowSurface::WindowSurface(EGLDisplay display,
                             EGLContext context,
                             EGLSurface surface,
                             size_t width,
                             size_t height)
    : Surface(display, context, surface), width_(width), height_(height) {}

bool WindowSurface::SetVSyncEnabled(bool enabled) {
  FML_DCHECK(IsCurrent());

  if (::eglSwapInterval(display_, enabled ? 1 : 0) != EGL_TRUE) {
    WINDOWS_LOG_EGL_ERROR;
    return false;
  }

  vsync_enabled_ = enabled;
  return true;
}

size_t WindowSurface::width() const {
  return width_;
}

size_t WindowSurface::height() const {
  return height_;
}

bool WindowSurface::vsync_enabled() const {
  return vsync_enabled_;
}

}  // namespace egl
}  // namespace appcode
