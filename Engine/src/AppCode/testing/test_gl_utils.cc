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

#include "appcode/testing/test_gl_utils.h"

#include <EGL/egl.h>

#include <sstream>

namespace appcode::testing {

std::string GetEGLError() {
  std::stringstream stream;

  auto error = ::eglGetError();

  stream << "EGL Result: '";

  switch (error) {
    case EGL_SUCCESS:
      stream << "EGL_SUCCESS";
      break;
    case EGL_NOT_INITIALIZED:
      stream << "EGL_NOT_INITIALIZED";
      break;
    case EGL_BAD_ACCESS:
      stream << "EGL_BAD_ACCESS";
      break;
    case EGL_BAD_ALLOC:
      stream << "EGL_BAD_ALLOC";
      break;
    case EGL_BAD_ATTRIBUTE:
      stream << "EGL_BAD_ATTRIBUTE";
      break;
    case EGL_BAD_CONTEXT:
      stream << "EGL_BAD_CONTEXT";
      break;
    case EGL_BAD_CONFIG:
      stream << "EGL_BAD_CONFIG";
      break;
    case EGL_BAD_CURRENT_SURFACE:
      stream << "EGL_BAD_CURRENT_SURFACE";
      break;
    case EGL_BAD_DISPLAY:
      stream << "EGL_BAD_DISPLAY";
      break;
    case EGL_BAD_SURFACE:
      stream << "EGL_BAD_SURFACE";
      break;
    case EGL_BAD_MATCH:
      stream << "EGL_BAD_MATCH";
      break;
    case EGL_BAD_PARAMETER:
      stream << "EGL_BAD_PARAMETER";
      break;
    case EGL_BAD_NATIVE_PIXMAP:
      stream << "EGL_BAD_NATIVE_PIXMAP";
      break;
    case EGL_BAD_NATIVE_WINDOW:
      stream << "EGL_BAD_NATIVE_WINDOW";
      break;
    case EGL_CONTEXT_LOST:
      stream << "EGL_CONTEXT_LOST";
      break;
    default:
      stream << "Unknown";
  }

  stream << "' (0x" << std::hex << error << std::dec << ").";
  return stream.str();
}

}  // namespace appcode::testing
