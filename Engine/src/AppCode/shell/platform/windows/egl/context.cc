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

#include "appcode/shell/platform/windows/egl/context.h"

#include "appcode/shell/platform/windows/egl/egl.h"

namespace appcode {
namespace egl {

Context::Context(EGLDisplay display, EGLContext context)
    : display_(display), context_(context) {}

Context::~Context() {
  if (display_ == EGL_NO_DISPLAY && context_ == EGL_NO_CONTEXT) {
    return;
  }

  if (::eglDestroyContext(display_, context_) != EGL_TRUE) {
    WINDOWS_LOG_EGL_ERROR;
  }
}

bool Context::IsCurrent() const {
  return ::eglGetCurrentContext() == context_;
}

bool Context::MakeCurrent() const {
  const auto result =
      ::eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_);
  if (result != EGL_TRUE) {
    WINDOWS_LOG_EGL_ERROR;
    return false;
  }

  return true;
}

bool Context::ClearCurrent() const {
  const auto result = ::eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                                       EGL_NO_CONTEXT);
  if (result != EGL_TRUE) {
    WINDOWS_LOG_EGL_ERROR;
    return false;
  }

  return true;
}

const EGLContext& Context::GetHandle() const {
  return context_;
}

}  // namespace egl
}  // namespace appcode
