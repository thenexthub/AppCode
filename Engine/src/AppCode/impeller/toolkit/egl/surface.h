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

#ifndef APPCODE_IMPELLER_TOOLKIT_EGL_SURFACE_H_
#define APPCODE_IMPELLER_TOOLKIT_EGL_SURFACE_H_

#include "impeller/toolkit/egl/egl.h"

namespace impeller {
namespace egl {

//------------------------------------------------------------------------------
/// @brief      An instance of an EGL surface. There is no ability to create
///             surfaces directly. Instead, one must be created using a Display
///             connection.
///
class Surface {
 public:
  ~Surface();

  //----------------------------------------------------------------------------
  /// @return     True if this is a valid surface.
  ///
  bool IsValid() const;

  //----------------------------------------------------------------------------
  /// @return     Get the handle to the underlying surface.
  ///
  const EGLSurface& GetHandle() const;

  //----------------------------------------------------------------------------
  /// @brief      Present the surface. For an offscreen pixel buffer surface,
  ///             this is a no-op.
  ///
  /// @return     True if the surface could be presented.
  ///
  bool Present() const;

 private:
  friend class Display;

  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLSurface surface_ = EGL_NO_SURFACE;

  Surface(EGLDisplay display, EGLSurface surface);

  Surface(const Surface&) = delete;

  Surface& operator=(const Surface&) = delete;
};

}  // namespace egl
}  // namespace impeller

#endif  // APPCODE_IMPELLER_TOOLKIT_EGL_SURFACE_H_
