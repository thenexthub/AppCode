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

#ifndef APPCODE_IMPELLER_RENDERER_SHADER_KEY_H_
#define APPCODE_IMPELLER_RENDERER_SHADER_KEY_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "appcode/fml/hash_combine.h"
#include "impeller/core/shader_types.h"

namespace impeller {

struct ShaderKey {
  std::string name;
  ShaderStage stage = ShaderStage::kUnknown;

  ShaderKey(std::string_view p_name, ShaderStage p_stage)
      : name({p_name.data(), p_name.size()}), stage(p_stage) {}

  struct Hash {
    size_t operator()(const ShaderKey& key) const {
      return fml::HashCombine(key.name, key.stage);
    }
  };

  struct Equal {
    constexpr bool operator()(const ShaderKey& k1, const ShaderKey& k2) const {
      return k1.stage == k2.stage && k1.name == k2.name;
    }
  };
};

class ShaderFunction;

using ShaderFunctionMap =
    std::unordered_map<ShaderKey,
                       std::shared_ptr<const ShaderFunction>,
                       ShaderKey::Hash,
                       ShaderKey::Equal>;

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_SHADER_KEY_H_
