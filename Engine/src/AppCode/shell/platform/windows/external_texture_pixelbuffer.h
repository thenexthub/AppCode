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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_EXTERNAL_TEXTURE_PIXELBUFFER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_EXTERNAL_TEXTURE_PIXELBUFFER_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/public/appcode_texture_registrar.h"
#include "appcode/shell/platform/windows/egl/proc_table.h"
#include "appcode/shell/platform/windows/external_texture.h"

namespace appcode {

// An abstraction of an pixel-buffer based texture.
class ExternalTexturePixelBuffer : public ExternalTexture {
 public:
  ExternalTexturePixelBuffer(
      const FlutterDesktopPixelBufferTextureCallback texture_callback,
      void* user_data,
      std::shared_ptr<egl::ProcTable> gl);

  virtual ~ExternalTexturePixelBuffer();

  // |ExternalTexture|
  bool PopulateTexture(size_t width,
                       size_t height,
                       FlutterOpenGLTexture* opengl_texture) override;

 private:
  // Attempts to copy the pixel buffer returned by |texture_callback_| to
  // OpenGL.
  // The |width| and |height| will be set to the actual bounds of the copied
  // pixel buffer.
  // Returns true on success or false if the pixel buffer returned
  // by |texture_callback_| was invalid.
  bool CopyPixelBuffer(size_t& width, size_t& height);

  const FlutterDesktopPixelBufferTextureCallback texture_callback_ = nullptr;
  void* const user_data_ = nullptr;
  std::shared_ptr<egl::ProcTable> gl_;
  GLuint gl_texture_ = 0;

  FML_DISALLOW_COPY_AND_ASSIGN(ExternalTexturePixelBuffer);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_EXTERNAL_TEXTURE_PIXELBUFFER_H_
