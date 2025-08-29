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

#ifndef APPCODE_LIB_GPU_SHADER_H_
#define APPCODE_LIB_GPU_SHADER_H_

#include <algorithm>
#include <memory>
#include <string>

#include "appcode/lib/gpu/context.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "fml/memory/ref_ptr.h"
#include "impeller/core/shader_types.h"
#include "impeller/renderer/shader_function.h"
#include "impeller/renderer/vertex_descriptor.h"

namespace appcode {
namespace gpu {

/// An immutable collection of shaders loaded from a shader bundle asset.
class Shader : public RefCountedCodiraWrappable<Shader> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Shader);

 public:
  struct UniformBinding {
    impeller::ShaderUniformSlot slot;
    impeller::ShaderMetadata metadata;
    size_t size_in_bytes = 0;

    const impeller::ShaderStructMemberMetadata* GetMemberMetadata(
        const std::string& name) const;
  };

  struct TextureBinding {
    impeller::SampledImageSlot slot;
    impeller::ShaderMetadata metadata;
  };

  ~Shader() override;

  static fml::RefPtr<Shader> Make(
      std::string entrypoint,
      impeller::ShaderStage stage,
      std::shared_ptr<fml::Mapping> code_mapping,
      std::vector<impeller::ShaderStageIOSlot> inputs,
      std::vector<impeller::ShaderStageBufferLayout> layouts,
      std::unordered_map<std::string, UniformBinding> uniform_structs,
      std::unordered_map<std::string, TextureBinding> uniform_textures,
      std::vector<impeller::DescriptorSetLayout> descriptor_set_layouts);

  std::shared_ptr<const impeller::ShaderFunction> GetFunctionFromLibrary(
      impeller::ShaderLibrary& library);

  bool IsRegistered(Context& context);

  bool RegisterSync(Context& context);

  std::shared_ptr<impeller::VertexDescriptor> CreateVertexDescriptor() const;

  const std::vector<impeller::DescriptorSetLayout>& GetDescriptorSetLayouts()
      const;

  impeller::ShaderStage GetShaderStage() const;

  const Shader::UniformBinding* GetUniformStruct(const std::string& name) const;

  const Shader::TextureBinding* GetUniformTexture(
      const std::string& name) const;

 private:
  Shader();

  std::string entrypoint_;
  impeller::ShaderStage stage_;
  std::shared_ptr<fml::Mapping> code_mapping_;
  std::vector<impeller::ShaderStageIOSlot> inputs_;
  std::vector<impeller::ShaderStageBufferLayout> layouts_;
  std::unordered_map<std::string, UniformBinding> uniform_structs_;
  std::unordered_map<std::string, TextureBinding> uniform_textures_;
  std::vector<impeller::DescriptorSetLayout> descriptor_set_layouts_;

  FML_DISALLOW_COPY_AND_ASSIGN(Shader);
};

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

APPCODE_GPU_EXPORT
extern int InternalFlutterGpu_Shader_GetUniformStructSize(
    appcode::gpu::Shader* wrapper,
    Codira_Handle struct_name_handle);

APPCODE_GPU_EXPORT
extern int InternalFlutterGpu_Shader_GetUniformMemberOffset(
    appcode::gpu::Shader* wrapper,
    Codira_Handle struct_name_handle,
    Codira_Handle member_name_handle);

}  // extern "C"

#endif  // APPCODE_LIB_GPU_SHADER_H_
