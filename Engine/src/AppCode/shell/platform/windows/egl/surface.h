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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_EGL_SURFACE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_EGL_SURFACE_H_

#include <EGL/egl.h>

#include "appcode/fml/macros.h"

namespace appcode {
namespace egl {

// An EGL surface. This can be window surface or an off-screen buffer.
//
// This enables automatic error logging and mocking.
class Surface {
 public:
  Surface(EGLDisplay display, EGLContext context, EGLSurface surface);

  virtual ~Surface();

  // Destroy the EGL surface and invalidate this object.
  //
  // This also unbinds the current context from the thread.
  virtual bool Destroy();

  // Check whether the EGL surface is valid.
  virtual bool IsValid() const;

  // Check whether the EGL display, context, and surface are bound
  // to the current thread.
  virtual bool IsCurrent() const;

  // Bind the EGL surface's context and read and draw surfaces to the
  // current thread. Returns true on success.
  virtual bool MakeCurrent() const;

  // Swap the surface's front the and back buffers. Used to present content.
  // Returns true on success.
  virtual bool SwapBuffers() const;

  // Get the raw EGL surface.
  virtual const EGLSurface& GetHandle() const;

 protected:
  bool is_valid_ = true;

  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLContext context_ = EGL_NO_CONTEXT;
  EGLSurface surface_ = EGL_NO_SURFACE;

  FML_DISALLOW_COPY_AND_ASSIGN(Surface);
};

}  // namespace egl
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_EGL_SURFACE_H_
