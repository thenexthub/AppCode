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

#ifndef APPCODE_IMPELLER_COMPILER_SOURCE_OPTIONS_H_
#define APPCODE_IMPELLER_COMPILER_SOURCE_OPTIONS_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "appcode/fml/unique_fd.h"
#include "impeller/compiler/include_dir.h"
#include "impeller/compiler/types.h"

namespace impeller {
namespace compiler {

struct SourceOptions {
  SourceType type = SourceType::kUnknown;
  TargetPlatform target_platform = TargetPlatform::kUnknown;
  SourceLanguage source_language = SourceLanguage::kUnknown;
  std::shared_ptr<fml::UniqueFD> working_directory;
  std::vector<IncludeDir> include_dirs;
  std::string file_name = "main.glsl";
  std::string entry_point_name = "main";
  uint32_t gles_language_version = 100;
  std::vector<std::string> defines;
  bool json_format = false;
  std::string metal_version;

  /// @brief Whether half-precision textures should be supported, requiring
  /// opengl semantics. Only used on metal targets.
  bool use_half_textures = false;

  /// @brief Whether the GLSL framebuffer fetch extension will be required.
  ///
  /// Only used on OpenGLES targets.
  bool require_framebuffer_fetch = false;

  SourceOptions();

  ~SourceOptions();

  explicit SourceOptions(const std::string& file_name,
                         SourceType source_type = SourceType::kUnknown);
};

}  // namespace compiler
}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_SOURCE_OPTIONS_H_
