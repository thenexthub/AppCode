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

#ifndef APPCODE_IMPELLER_SHADER_ARCHIVE_SHADER_ARCHIVE_WRITER_H_
#define APPCODE_IMPELLER_SHADER_ARCHIVE_SHADER_ARCHIVE_WRITER_H_

#include <memory>
#include <string>
#include <vector>

#include "appcode/fml/mapping.h"
#include "impeller/shader_archive/shader_archive_types.h"

namespace impeller {

class ShaderArchiveWriter {
 public:
  ShaderArchiveWriter();

  ~ShaderArchiveWriter();

  [[nodiscard]] bool AddShaderAtPath(const std::string& path);

  [[nodiscard]] bool AddShader(ArchiveShaderType type,
                               std::string name,
                               std::shared_ptr<fml::Mapping> mapping);

  std::shared_ptr<fml::Mapping> CreateMapping() const;

 private:
  struct ShaderDescription {
    ArchiveShaderType type;
    std::string name;
    std::shared_ptr<fml::Mapping> mapping;
  };

  std::vector<ShaderDescription> shader_descriptions_;

  ShaderArchiveWriter(const ShaderArchiveWriter&) = delete;

  ShaderArchiveWriter& operator=(const ShaderArchiveWriter&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_SHADER_ARCHIVE_SHADER_ARCHIVE_WRITER_H_
