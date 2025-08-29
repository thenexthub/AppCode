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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SHADER_LIBRARY_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SHADER_LIBRARY_GLES_H_

#include <memory>

#include "appcode/fml/mapping.h"
#include "impeller/base/comparable.h"
#include "impeller/base/thread.h"
#include "impeller/renderer/shader_key.h"
#include "impeller/renderer/shader_library.h"

namespace impeller {

class ShaderLibraryGLES final : public ShaderLibrary {
 public:
  // |ShaderLibrary|
  ~ShaderLibraryGLES() override;

  // |ShaderLibrary|
  bool IsValid() const override;

 private:
  friend class ContextGLES;
  const UniqueID library_id_;
  mutable RWMutex functions_mutex_;
  ShaderFunctionMap functions_ IPLR_GUARDED_BY(functions_mutex_);
  bool is_valid_ = false;

  explicit ShaderLibraryGLES(
      const std::vector<std::shared_ptr<fml::Mapping>>& shader_libraries);

  // |ShaderLibrary|
  std::shared_ptr<const ShaderFunction> GetFunction(std::string_view name,
                                                    ShaderStage stage) override;

  // |ShaderLibrary|
  void RegisterFunction(std::string name,
                        ShaderStage stage,
                        std::shared_ptr<fml::Mapping> code,
                        RegistrationCallback callback) override;

  // |ShaderLibrary|
  void UnregisterFunction(std::string name, ShaderStage stage) override;

  ShaderLibraryGLES(const ShaderLibraryGLES&) = delete;

  ShaderLibraryGLES& operator=(const ShaderLibraryGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SHADER_LIBRARY_GLES_H_
