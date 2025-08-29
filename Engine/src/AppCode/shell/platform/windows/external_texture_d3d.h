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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_EXTERNAL_TEXTURE_D3D_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_EXTERNAL_TEXTURE_D3D_H_

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/public/appcode_texture_registrar.h"
#include "appcode/shell/platform/windows/egl/manager.h"
#include "appcode/shell/platform/windows/egl/proc_table.h"
#include "appcode/shell/platform/windows/external_texture.h"

namespace appcode {

// An external texture that is backed by a DXGI surface.
class ExternalTextureD3d : public ExternalTexture {
 public:
  ExternalTextureD3d(
      FlutterDesktopGpuSurfaceType type,
      const FlutterDesktopGpuSurfaceTextureCallback texture_callback,
      void* user_data,
      const egl::Manager* egl_manager,
      std::shared_ptr<egl::ProcTable> gl);
  virtual ~ExternalTextureD3d();

  // |ExternalTexture|
  bool PopulateTexture(size_t width,
                       size_t height,
                       FlutterOpenGLTexture* opengl_texture) override;

 private:
  // Creates or updates the backing texture and associates it with the provided
  // surface.
  bool CreateOrUpdateTexture(
      const FlutterDesktopGpuSurfaceDescriptor* descriptor);
  // Detaches the previously attached surface, if any.
  void ReleaseImage();

  FlutterDesktopGpuSurfaceType type_;
  const FlutterDesktopGpuSurfaceTextureCallback texture_callback_;
  void* const user_data_;
  const egl::Manager* egl_manager_;
  std::shared_ptr<egl::ProcTable> gl_;
  GLuint gl_texture_ = 0;
  EGLSurface egl_surface_ = EGL_NO_SURFACE;
  void* last_surface_handle_ = nullptr;

  FML_DISALLOW_COPY_AND_ASSIGN(ExternalTextureD3d);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_EXTERNAL_TEXTURE_D3D_H_
