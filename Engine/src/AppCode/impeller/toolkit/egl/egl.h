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

#ifndef APPCODE_IMPELLER_TOOLKIT_EGL_EGL_H_
#define APPCODE_IMPELLER_TOOLKIT_EGL_EGL_H_

#include <EGL/egl.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

#include <functional>

namespace impeller {
namespace egl {

//------------------------------------------------------------------------------
/// @brief      Creates a proc address resolver that resolves function pointers
///             to EGL and OpenGL (ES) procs.
///
/// @return     The resolver if one can be created.
///
std::function<void*(const char*)> CreateProcAddressResolver();

#define IMPELLER_LOG_EGL_ERROR LogEGLError(__FILE__, __LINE__);

void LogEGLError(const char* file, int line);

}  // namespace egl
}  // namespace impeller

#endif  // APPCODE_IMPELLER_TOOLKIT_EGL_EGL_H_
