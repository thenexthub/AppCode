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

#include "impeller/renderer/shader_function.h"

namespace impeller {

ShaderFunction::ShaderFunction(UniqueID parent_library_id,
                               std::string name,
                               ShaderStage stage)
    : parent_library_id_(parent_library_id),
      name_(std::move(name)),
      stage_(stage) {}

ShaderFunction::~ShaderFunction() = default;

ShaderStage ShaderFunction::GetStage() const {
  return stage_;
}

const std::string& ShaderFunction::GetName() const {
  return name_;
}

// |Comparable<ShaderFunction>|
std::size_t ShaderFunction::GetHash() const {
  return fml::HashCombine(parent_library_id_, name_, stage_);
}

// |Comparable<ShaderFunction>|
bool ShaderFunction::IsEqual(const ShaderFunction& other) const {
  return parent_library_id_ == other.parent_library_id_ &&
         name_ == other.name_ && stage_ == other.stage_;
}

}  // namespace impeller
