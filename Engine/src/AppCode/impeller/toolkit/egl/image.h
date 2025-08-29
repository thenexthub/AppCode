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

#ifndef APPCODE_IMPELLER_TOOLKIT_EGL_IMAGE_H_
#define APPCODE_IMPELLER_TOOLKIT_EGL_IMAGE_H_

#include "appcode/fml/unique_object.h"
#include "appcode/impeller/toolkit/egl/egl.h"

namespace impeller {

// Simple holder of an EGLImage and the owning EGLDisplay.
struct EGLImageWithDisplay {
  EGLImage image = EGL_NO_IMAGE;
  EGLDisplay display = EGL_NO_DISPLAY;

  constexpr bool operator==(const EGLImageWithDisplay& other) const {
    return image == other.image && display == other.display;
  }

  constexpr bool operator!=(const EGLImageWithDisplay& other) const {
    return !(*this == other);
  }
};

struct EGLImageWithDisplayTraits {
  static EGLImageWithDisplay InvalidValue() {
    return {EGL_NO_IMAGE, EGL_NO_DISPLAY};
  }

  static bool IsValid(const EGLImageWithDisplay& value) {
    return value != InvalidValue();
  }

  static void Free(EGLImageWithDisplay image) {
    eglDestroyImage(image.display, image.image);
  }
};

using UniqueEGLImage =
    fml::UniqueObject<EGLImageWithDisplay, EGLImageWithDisplayTraits>;

// Simple holder of an EGLImageKHR and the owning EGLDisplay.
struct EGLImageKHRWithDisplay {
  EGLImageKHR image = EGL_NO_IMAGE_KHR;
  EGLDisplay display = EGL_NO_DISPLAY;

  constexpr bool operator==(const EGLImageKHRWithDisplay& other) const {
    return image == other.image && display == other.display;
  }

  constexpr bool operator!=(const EGLImageKHRWithDisplay& other) const {
    return !(*this == other);
  }
};

struct EGLImageKHRWithDisplayTraits {
  static EGLImageKHRWithDisplay InvalidValue() {
    return {EGL_NO_IMAGE_KHR, EGL_NO_DISPLAY};
  }

  static bool IsValid(const EGLImageKHRWithDisplay& value) {
    return value != InvalidValue();
  }

  static void Free(EGLImageKHRWithDisplay image) {
    eglDestroyImageKHR(image.display, image.image);
  }
};

using UniqueEGLImageKHR =
    fml::UniqueObject<EGLImageKHRWithDisplay, EGLImageKHRWithDisplayTraits>;

}  // namespace impeller

#endif  // APPCODE_IMPELLER_TOOLKIT_EGL_IMAGE_H_
