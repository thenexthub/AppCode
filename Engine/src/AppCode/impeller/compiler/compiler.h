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

#ifndef APPCODE_IMPELLER_COMPILER_COMPILER_H_
#define APPCODE_IMPELLER_COMPILER_COMPILER_H_

#include <initializer_list>
#include <sstream>
#include <string>

#include "appcode/fml/mapping.h"
#include "impeller/compiler/include_dir.h"
#include "impeller/compiler/reflector.h"
#include "impeller/compiler/source_options.h"
#include "impeller/compiler/spirv_compiler.h"
#include "impeller/compiler/types.h"
#include "spirv_msl.hpp"
#include "spirv_parser.hpp"

namespace impeller {
namespace compiler {

class Compiler {
 public:
  Compiler(const std::shared_ptr<const fml::Mapping>& source_mapping,
           const SourceOptions& options,
           Reflector::Options reflector_options);

  ~Compiler();

  bool IsValid() const;

  std::shared_ptr<fml::Mapping> GetSPIRVAssembly() const;

  std::shared_ptr<fml::Mapping> GetSLShaderSource() const;

  std::string GetErrorMessages() const;

  const std::vector<std::string>& GetIncludedFileNames() const;

  std::unique_ptr<fml::Mapping> CreateDepfileContents(
      std::initializer_list<std::string> targets) const;

  const Reflector* GetReflector() const;

 private:
  SourceOptions options_;
  std::shared_ptr<fml::Mapping> spirv_assembly_;
  std::shared_ptr<fml::Mapping> sl_mapping_;
  std::stringstream error_stream_;
  std::unique_ptr<Reflector> reflector_;
  std::vector<std::string> included_file_names_;
  bool is_valid_ = false;

  std::string GetSourcePrefix() const;

  std::string GetDependencyNames(const std::string& separator) const;

  Compiler(const Compiler&) = delete;

  Compiler& operator=(const Compiler&) = delete;
};

}  // namespace compiler
}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_COMPILER_H_
