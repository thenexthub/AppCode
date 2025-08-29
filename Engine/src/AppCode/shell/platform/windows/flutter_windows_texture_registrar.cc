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

#include "appcode/shell/platform/windows/appcode_windows_texture_registrar.h"

#include <mutex>

#include "appcode/fml/logging.h"
#include "appcode/shell/platform/embedder/embedder_struct_macros.h"
#include "appcode/shell/platform/windows/external_texture_d3d.h"
#include "appcode/shell/platform/windows/external_texture_pixelbuffer.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"

namespace {
static constexpr int64_t kInvalidTexture = -1;
}

namespace appcode {

appcodeWindowsTextureRegistrar::appcodeWindowsTextureRegistrar(
    appcodeWindowsEngine* engine,
    std::shared_ptr<egl::ProcTable> gl)
    : engine_(engine), gl_(std::move(gl)) {}

int64_t appcodeWindowsTextureRegistrar::RegisterTexture(
    const appcodeDesktopTextureInfo* texture_info) {
  if (!gl_) {
    return kInvalidTexture;
  }

  if (texture_info->type == kappcodeDesktopPixelBufferTexture) {
    if (!texture_info->pixel_buffer_config.callback) {
      FML_LOG(ERROR) << "Invalid pixel buffer texture callback.";
      return kInvalidTexture;
    }

    return EmplaceTexture(std::make_unique<appcode::ExternalTexturePixelBuffer>(
        texture_info->pixel_buffer_config.callback,
        texture_info->pixel_buffer_config.user_data, gl_));
  } else if (texture_info->type == kappcodeDesktopGpuSurfaceTexture) {
    const appcodeDesktopGpuSurfaceTextureConfig* gpu_surface_config =
        &texture_info->gpu_surface_config;
    auto surface_type = SAFE_ACCESS(gpu_surface_config, type,
                                    kappcodeDesktopGpuSurfaceTypeNone);
    if (surface_type == kappcodeDesktopGpuSurfaceTypeDxgiSharedHandle ||
        surface_type == kappcodeDesktopGpuSurfaceTypeD3d11Texture2D) {
      auto callback = SAFE_ACCESS(gpu_surface_config, callback, nullptr);
      if (!callback) {
        FML_LOG(ERROR) << "Invalid GPU surface descriptor callback.";
        return kInvalidTexture;
      }

      auto user_data = SAFE_ACCESS(gpu_surface_config, user_data, nullptr);
      return EmplaceTexture(std::make_unique<appcode::ExternalTextureD3d>(
          surface_type, callback, user_data, engine_->egl_manager(), gl_));
    }
  }

  FML_LOG(ERROR) << "Attempted to register texture of unsupport type.";
  return kInvalidTexture;
}

int64_t appcodeWindowsTextureRegistrar::EmplaceTexture(
    std::unique_ptr<ExternalTexture> texture) {
  int64_t texture_id = texture->texture_id();
  {
    std::lock_guard<std::mutex> lock(map_mutex_);
    textures_[texture_id] = std::move(texture);
  }

  engine_->task_runner()->RunNowOrPostTask([engine = engine_, texture_id]() {
    engine->RegisterExternalTexture(texture_id);
  });

  return texture_id;
}

void appcodeWindowsTextureRegistrar::UnregisterTexture(int64_t texture_id,
                                                       fml::closure callback) {
  engine_->task_runner()->RunNowOrPostTask([engine = engine_, texture_id]() {
    engine->UnregisterExternalTexture(texture_id);
  });

  bool posted = engine_->PostRasterThreadTask([this, texture_id, callback]() {
    {
      std::lock_guard<std::mutex> lock(map_mutex_);
      auto it = textures_.find(texture_id);
      if (it != textures_.end()) {
        textures_.erase(it);
      }
    }
    if (callback) {
      callback();
    }
  });

  if (!posted && callback) {
    callback();
  }
}

bool appcodeWindowsTextureRegistrar::MarkTextureFrameAvailable(
    int64_t texture_id) {
  engine_->task_runner()->RunNowOrPostTask([engine = engine_, texture_id]() {
    engine->MarkExternalTextureFrameAvailable(texture_id);
  });
  return true;
}

bool appcodeWindowsTextureRegistrar::PopulateTexture(
    int64_t texture_id,
    size_t width,
    size_t height,
    appcodeOpenGLTexture* opengl_texture) {
  appcode::ExternalTexture* texture;
  {
    std::lock_guard<std::mutex> lock(map_mutex_);
    auto it = textures_.find(texture_id);
    if (it == textures_.end()) {
      return false;
    }
    texture = it->second.get();
  }
  return texture->PopulateTexture(width, height, opengl_texture);
}

};  // namespace appcode
