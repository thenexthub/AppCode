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

#include "impeller/renderer/shader_library.h"

namespace impeller {

ShaderLibrary::ShaderLibrary() = default;

ShaderLibrary::~ShaderLibrary() = default;

void ShaderLibrary::RegisterFunction(
    std::string name,  // NOLINT(performance-unnecessary-value-param)
    ShaderStage stage,
    std::shared_ptr<fml::Mapping>
        code,  // NOLINT(performance-unnecessary-value-param)
    RegistrationCallback
        callback) {  // NOLINT(performance-unnecessary-value-param)
  if (callback) {
    callback(false);
  }
}

}  // namespace impeller
