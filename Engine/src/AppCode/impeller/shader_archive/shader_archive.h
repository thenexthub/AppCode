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

#ifndef APPCODE_IMPELLER_SHADER_ARCHIVE_SHADER_ARCHIVE_H_
#define APPCODE_IMPELLER_SHADER_ARCHIVE_SHADER_ARCHIVE_H_

#include <memory>
#include <type_traits>
#include <unordered_map>

#include "appcode/fml/hash_combine.h"
#include "appcode/fml/mapping.h"
#include "impeller/shader_archive/shader_archive_types.h"

namespace impeller {

class ShaderArchive {
 public:
  explicit ShaderArchive(std::shared_ptr<fml::Mapping> payload);

  ShaderArchive(ShaderArchive&&);

  ~ShaderArchive();

  bool IsValid() const;

  size_t GetShaderCount() const;

  std::shared_ptr<fml::Mapping> GetMapping(ArchiveShaderType type,
                                           std::string name) const;

  size_t IterateAllShaders(
      const std::function<bool(ArchiveShaderType type,
                               const std::string& name,
                               const std::shared_ptr<fml::Mapping>& mapping)>&)
      const;

 private:
  struct ShaderKey {
    ArchiveShaderType type = ArchiveShaderType::kFragment;
    std::string name;

    struct Hash {
      size_t operator()(const ShaderKey& key) const {
        return fml::HashCombine(
            static_cast<std::underlying_type_t<decltype(key.type)>>(key.type),
            key.name);
      }
    };

    struct Equal {
      bool operator()(const ShaderKey& lhs, const ShaderKey& rhs) const {
        return lhs.type == rhs.type && lhs.name == rhs.name;
      }
    };
  };

  using Shaders = std::unordered_map<ShaderKey,
                                     std::shared_ptr<fml::Mapping>,
                                     ShaderKey::Hash,
                                     ShaderKey::Equal>;

  std::shared_ptr<fml::Mapping> payload_;
  Shaders shaders_;
  bool is_valid_ = false;

  ShaderArchive(const ShaderArchive&) = delete;

  ShaderArchive& operator=(const ShaderArchive&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_SHADER_ARCHIVE_SHADER_ARCHIVE_H_
