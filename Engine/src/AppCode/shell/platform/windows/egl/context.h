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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_EGL_CONTEXT_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_EGL_CONTEXT_H_

#include <EGL/egl.h>

#include "appcode/fml/macros.h"

namespace appcode {
namespace egl {

// An EGL context to interact with OpenGL.
//
// This enables automatic error logging and mocking.
//
// Flutter Windows uses this to create render and resource contexts.
class Context {
 public:
  Context(EGLDisplay display, EGLContext context);
  ~Context();

  // Check if this context is currently bound to the thread.
  virtual bool IsCurrent() const;

  // Bind the context to the thread without any read or draw surfaces.
  //
  // Returns true on success.
  virtual bool MakeCurrent() const;

  // Unbind any context and surfaces from the thread.
  //
  // Returns true on success.
  virtual bool ClearCurrent() const;

  // Get the raw EGL context.
  virtual const EGLContext& GetHandle() const;

 private:
  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLContext context_ = EGL_NO_CONTEXT;

  FML_DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace egl
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_EGL_CONTEXT_H_
