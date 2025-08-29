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

#include "impeller/renderer/backend/gles/shader_function_gles.h"

namespace impeller {

ShaderFunctionGLES::ShaderFunctionGLES(
    UniqueID library_id,
    ShaderStage stage,
    std::string name,
    std::shared_ptr<const fml::Mapping> mapping)
    : ShaderFunction(library_id, std::move(name), stage),
      mapping_(std::move(mapping)) {}

ShaderFunctionGLES::~ShaderFunctionGLES() = default;

const std::shared_ptr<const fml::Mapping>&
ShaderFunctionGLES::GetSourceMapping() const {
  return mapping_;
}

}  // namespace impeller
