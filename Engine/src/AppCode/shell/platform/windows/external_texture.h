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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_EXTERNAL_TEXTURE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_EXTERNAL_TEXTURE_H_

#include "appcode/shell/platform/embedder/embedder.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace appcode {

// Abstract external texture.
class ExternalTexture {
 public:
  virtual ~ExternalTexture() = default;

  // Returns the unique id of this texture.
  int64_t texture_id() const { return reinterpret_cast<int64_t>(this); };

  // Attempts to populate the specified |opengl_texture| with texture details
  // such as the name, width, height and the pixel format.
  // Returns true on success.
  virtual bool PopulateTexture(size_t width,
                               size_t height,
                               FlutterOpenGLTexture* opengl_texture) = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_EXTERNAL_TEXTURE_H_
