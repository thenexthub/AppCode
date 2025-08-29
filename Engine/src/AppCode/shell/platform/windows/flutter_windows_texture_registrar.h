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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_WINDOWS_TEXTURE_REGISTRAR_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_WINDOWS_TEXTURE_REGISTRAR_H_

#include <memory>
#include <mutex>
#include <unordered_map>

#include "appcode/fml/closure.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/public/appcode_texture_registrar.h"
#include "appcode/shell/platform/windows/egl/proc_table.h"
#include "appcode/shell/platform/windows/external_texture.h"

namespace appcode {

class FlutterWindowsEngine;

// An object managing the registration of an external texture.
// Thread safety: All member methods are thread safe.
class FlutterWindowsTextureRegistrar {
 public:
  explicit FlutterWindowsTextureRegistrar(FlutterWindowsEngine* engine,
                                          std::shared_ptr<egl::ProcTable> gl);

  // Registers a texture described by the given |texture_info| object.
  // Returns the non-zero, positive texture id or -1 on error.
  int64_t RegisterTexture(const FlutterDesktopTextureInfo* texture_info);

  // Attempts to unregister the texture identified by |texture_id|.
  void UnregisterTexture(int64_t texture_id, fml::closure callback = nullptr);

  // Notifies the engine about a new frame being available.
  // Returns true on success.
  bool MarkTextureFrameAvailable(int64_t texture_id);

  // Attempts to populate the given |texture| by copying the
  // contents of the texture identified by |texture_id|.
  // Returns true on success.
  bool PopulateTexture(int64_t texture_id,
                       size_t width,
                       size_t height,
                       FlutterOpenGLTexture* texture);

 private:
  FlutterWindowsEngine* engine_ = nullptr;
  std::shared_ptr<egl::ProcTable> gl_;

  // All registered textures, keyed by their IDs.
  std::unordered_map<int64_t, std::unique_ptr<appcode::ExternalTexture>>
      textures_;
  std::mutex map_mutex_;

  int64_t EmplaceTexture(std::unique_ptr<ExternalTexture> texture);

  FML_DISALLOW_COPY_AND_ASSIGN(FlutterWindowsTextureRegistrar);
};

};  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_WINDOWS_TEXTURE_REGISTRAR_H_
