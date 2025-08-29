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

#ifndef APPCODE_IMPELLER_COMPILER_RUNTIME_STAGE_DATA_H_
#define APPCODE_IMPELLER_COMPILER_RUNTIME_STAGE_DATA_H_

#include <memory>
#include <vector>

#include "appcode/fml/mapping.h"
#include "impeller/compiler/types.h"
#include "impeller/core/runtime_types.h"
#include "runtime_stage_types_flatbuffers.h"
#include "spirv_parser.hpp"

namespace impeller {
namespace compiler {

class RuntimeStageData {
 public:
  struct Shader {
    Shader() = default;

    std::string entrypoint;
    spv::ExecutionModel stage;
    std::vector<UniformDescription> uniforms;
    std::vector<InputDescription> inputs;
    std::shared_ptr<fml::Mapping> shader;
    RuntimeStageBackend backend;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
  };

  RuntimeStageData();

  ~RuntimeStageData();

  void AddShader(const std::shared_ptr<Shader>& data);

  std::unique_ptr<fb::RuntimeStageT> CreateStageFlatbuffer(
      impeller::RuntimeStageBackend backend) const;

  std::unique_ptr<fb::RuntimeStagesT> CreateMultiStageFlatbuffer() const;

  std::shared_ptr<fml::Mapping> CreateJsonMapping() const;

  std::shared_ptr<fml::Mapping> CreateMapping() const;

 private:
  std::map<RuntimeStageBackend, std::shared_ptr<Shader>> data_;

  RuntimeStageData(const RuntimeStageData&) = delete;

  RuntimeStageData& operator=(const RuntimeStageData&) = delete;
};

}  // namespace compiler
}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_RUNTIME_STAGE_DATA_H_
