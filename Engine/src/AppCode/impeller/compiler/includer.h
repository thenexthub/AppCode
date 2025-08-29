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

#ifndef APPCODE_IMPELLER_COMPILER_INCLUDER_H_
#define APPCODE_IMPELLER_COMPILER_INCLUDER_H_

#include <string>

#include "appcode/fml/mapping.h"
#include "impeller/compiler/include_dir.h"
#include "shaderc/shaderc.hpp"

namespace impeller {
namespace compiler {

struct IncluderData {
  std::string file_name;
  std::unique_ptr<fml::Mapping> mapping;

  IncluderData(std::string p_file_name, std::unique_ptr<fml::Mapping> p_mapping)
      : file_name(std::move(p_file_name)), mapping(std::move(p_mapping)) {}
};

class Includer final : public shaderc::CompileOptions::IncluderInterface {
 public:
  Includer(std::shared_ptr<fml::UniqueFD> working_directory,
           std::vector<IncludeDir> include_dirs,
           std::function<void(std::string)> on_file_included);

  // |shaderc::CompileOptions::IncluderInterface|
  ~Includer() override;

  // |shaderc::CompileOptions::IncluderInterface|
  shaderc_include_result* GetInclude(const char* requested_source,
                                     shaderc_include_type type,
                                     const char* requesting_source,
                                     size_t include_depth) override;

  // |shaderc::CompileOptions::IncluderInterface|
  void ReleaseInclude(shaderc_include_result* data) override;

 private:
  std::shared_ptr<fml::UniqueFD> working_directory_;
  std::vector<IncludeDir> include_dirs_;
  std::function<void(std::string)> on_file_included_;

  std::unique_ptr<fml::FileMapping> TryOpenMapping(
      const IncludeDir& dir,
      const char* requested_source);

  std::unique_ptr<fml::FileMapping> FindFirstMapping(
      const char* requested_source);

  Includer(const Includer&) = delete;

  Includer& operator=(const Includer&) = delete;
};

}  // namespace compiler
}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_INCLUDER_H_
