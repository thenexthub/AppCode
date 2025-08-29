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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SHADER_FUNCTION_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SHADER_FUNCTION_GLES_H_

#include <string>

#include "appcode/fml/mapping.h"
#include "impeller/base/backend_cast.h"
#include "impeller/renderer/shader_function.h"

namespace impeller {

class ShaderLibraryGLES;

class ShaderFunctionGLES final
    : public ShaderFunction,
      public BackendCast<ShaderFunctionGLES, ShaderFunction> {
 public:
  // |ShaderFunction|
  ~ShaderFunctionGLES() override;

  const std::shared_ptr<const fml::Mapping>& GetSourceMapping() const;

 private:
  friend ShaderLibraryGLES;

  std::shared_ptr<const fml::Mapping> mapping_;

  ShaderFunctionGLES(UniqueID library_id,
                     ShaderStage stage,
                     std::string name,
                     std::shared_ptr<const fml::Mapping> mapping);

  ShaderFunctionGLES(const ShaderFunctionGLES&) = delete;

  ShaderFunctionGLES& operator=(const ShaderFunctionGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_SHADER_FUNCTION_GLES_H_
