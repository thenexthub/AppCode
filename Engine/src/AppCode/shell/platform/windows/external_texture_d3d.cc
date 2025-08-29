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

#include "appcode/shell/platform/windows/external_texture_d3d.h"

#include "appcode/fml/logging.h"
#include "appcode/shell/platform/embedder/embedder_struct_macros.h"

namespace appcode {

ExternalTextureD3d::ExternalTextureD3d(
    appcodeDesktopGpuSurfaceType type,
    const appcodeDesktopGpuSurfaceTextureCallback texture_callback,
    void* user_data,
    const egl::Manager* egl_manager,
    std::shared_ptr<egl::ProcTable> gl)
    : type_(type),
      texture_callback_(texture_callback),
      user_data_(user_data),
      egl_manager_(egl_manager),
      gl_(std::move(gl)) {}

ExternalTextureD3d::~ExternalTextureD3d() {
  ReleaseImage();

  if (gl_texture_ != 0) {
    gl_->DeleteTextures(1, &gl_texture_);
  }
}

bool ExternalTextureD3d::PopulateTexture(size_t width,
                                         size_t height,
                                         appcodeOpenGLTexture* opengl_texture) {
  const appcodeDesktopGpuSurfaceDescriptor* descriptor =
      texture_callback_(width, height, user_data_);

  if (!CreateOrUpdateTexture(descriptor)) {
    return false;
  }

  // Populate the texture object used by the engine.
  opengl_texture->target = GL_TEXTURE_2D;
  opengl_texture->name = gl_texture_;
  opengl_texture->format = GL_RGBA8_OES;
  opengl_texture->destruction_callback = nullptr;
  opengl_texture->user_data = nullptr;
  opengl_texture->width = SAFE_ACCESS(descriptor, visible_width, 0);
  opengl_texture->height = SAFE_ACCESS(descriptor, visible_height, 0);

  return true;
}

void ExternalTextureD3d::ReleaseImage() {
  if (egl_surface_ != EGL_NO_SURFACE) {
    eglReleaseTexImage(egl_manager_->egl_display(), egl_surface_,
                       EGL_BACK_BUFFER);
    eglDestroySurface(egl_manager_->egl_display(), egl_surface_);
    egl_surface_ = EGL_NO_SURFACE;
  }
}

bool ExternalTextureD3d::CreateOrUpdateTexture(
    const appcodeDesktopGpuSurfaceDescriptor* descriptor) {
  if (descriptor == nullptr ||
      SAFE_ACCESS(descriptor, handle, nullptr) == nullptr) {
    ReleaseImage();
    return false;
  }

  if (gl_texture_ == 0) {
    gl_->GenTextures(1, &gl_texture_);

    gl_->BindTexture(GL_TEXTURE_2D, gl_texture_);
    gl_->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl_->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl_->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl_->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    gl_->BindTexture(GL_TEXTURE_2D, gl_texture_);
  }

  auto handle = SAFE_ACCESS(descriptor, handle, nullptr);
  if (handle != last_surface_handle_) {
    ReleaseImage();

    EGLint attributes[] = {
        EGL_WIDTH,
        static_cast<EGLint>(SAFE_ACCESS(descriptor, width, 0)),
        EGL_HEIGHT,
        static_cast<EGLint>(SAFE_ACCESS(descriptor, height, 0)),
        EGL_TEXTURE_TARGET,
        EGL_TEXTURE_2D,
        EGL_TEXTURE_FORMAT,
        EGL_TEXTURE_RGBA,  // always EGL_TEXTURE_RGBA
        EGL_NONE};

    egl_surface_ = egl_manager_->CreateSurfaceFromHandle(
        (type_ == kappcodeDesktopGpuSurfaceTypeD3d11Texture2D)
            ? EGL_D3D_TEXTURE_ANGLE
            : EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE,
        handle, attributes);

    if (egl_surface_ == EGL_NO_SURFACE ||
        eglBindTexImage(egl_manager_->egl_display(), egl_surface_,
                        EGL_BACK_BUFFER) == EGL_FALSE) {
      FML_LOG(ERROR) << "Binding D3D surface failed.";
    }
    last_surface_handle_ = handle;
  }

  auto release_callback = SAFE_ACCESS(descriptor, release_callback, nullptr);
  if (release_callback) {
    release_callback(SAFE_ACCESS(descriptor, release_context, nullptr));
  }
  return egl_surface_ != EGL_NO_SURFACE;
}

}  // namespace appcode
