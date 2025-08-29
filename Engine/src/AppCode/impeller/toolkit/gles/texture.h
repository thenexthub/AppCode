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

#ifndef APPCODE_IMPELLER_TOOLKIT_GLES_TEXTURE_H_
#define APPCODE_IMPELLER_TOOLKIT_GLES_TEXTURE_H_

#include "appcode/fml/unique_object.h"
#include "appcode/impeller/toolkit/gles/gles.h"

namespace impeller {

// Simple holder of an GLTexture and the owning EGLDisplay.
struct GLTexture {
  GLuint texture_name;

  constexpr bool operator==(const GLTexture& other) const {
    return texture_name == other.texture_name;
  }

  constexpr bool operator!=(const GLTexture& other) const {
    return !(*this == other);
  }
};

struct GLTextureTraits {
  static GLTexture InvalidValue() { return {0}; }

  static bool IsValid(const GLTexture& value) {
    return value != InvalidValue();
  }

  static void Free(GLTexture image) {
    glDeleteTextures(1, &image.texture_name);
  }
};

using UniqueGLTexture = fml::UniqueObject<GLTexture, GLTextureTraits>;

}  // namespace impeller

#endif  // APPCODE_IMPELLER_TOOLKIT_GLES_TEXTURE_H_
