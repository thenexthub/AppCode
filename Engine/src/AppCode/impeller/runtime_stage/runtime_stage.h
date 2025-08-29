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

#ifndef APPCODE_IMPELLER_RUNTIME_STAGE_RUNTIME_STAGE_H_
#define APPCODE_IMPELLER_RUNTIME_STAGE_RUNTIME_STAGE_H_

#include <map>
#include <memory>
#include <string>

#include "appcode/fml/mapping.h"

#include "appcode/impeller/core/runtime_types.h"
#include "impeller/core/shader_types.h"
#include "runtime_stage_types_flatbuffers.h"

namespace impeller {

class RuntimeStage {
 public:
  static const char* kVulkanUBOName;

  using Map = std::map<RuntimeStageBackend, std::shared_ptr<RuntimeStage>>;
  static Map DecodeRuntimeStages(const std::shared_ptr<fml::Mapping>& payload);

  RuntimeStage(const fb::RuntimeStage* runtime_stage,
               const std::shared_ptr<fml::Mapping>& payload);
  ~RuntimeStage();
  RuntimeStage(RuntimeStage&&);
  RuntimeStage& operator=(RuntimeStage&&);

  bool IsValid() const;

  RuntimeShaderStage GetShaderStage() const;

  const std::vector<RuntimeUniformDescription>& GetUniforms() const;

  const std::vector<DescriptorSetLayout>& GetDescriptorSetLayouts() const;

  const std::string& GetEntrypoint() const;

  const RuntimeUniformDescription* GetUniform(const std::string& name) const;

  const std::shared_ptr<fml::Mapping>& GetCodeMapping() const;

  bool IsDirty() const;

  void SetClean();

 private:
  std::shared_ptr<fml::Mapping> payload_;
  RuntimeShaderStage stage_ = RuntimeShaderStage::kVertex;
  std::string entrypoint_;
  std::shared_ptr<fml::Mapping> code_mapping_;
  std::vector<RuntimeUniformDescription> uniforms_;
  std::vector<DescriptorSetLayout> descriptor_set_layouts_;
  bool is_valid_ = false;
  bool is_dirty_ = true;

  RuntimeStage(const RuntimeStage&) = delete;

  static std::unique_ptr<RuntimeStage> RuntimeStageIfPresent(
      const fb::RuntimeStage* runtime_stage,
      const std::shared_ptr<fml::Mapping>& payload);

  RuntimeStage& operator=(const RuntimeStage&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RUNTIME_STAGE_RUNTIME_STAGE_H_
