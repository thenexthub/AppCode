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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_EGL_EGL_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_EGL_EGL_H_

#include <string_view>

namespace appcode {
namespace egl {

/// Log the last EGL error with an error message.
void LogEGLError(std::string_view message);

/// Log the last EGL error.
void LogEGLError(std::string_view file, int line);

#define WINDOWS_LOG_EGL_ERROR LogEGLError(__FILE__, __LINE__);

}  // namespace egl
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_EGL_EGL_H_
