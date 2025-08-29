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

#include "appcode/lib/gpu/render_pipeline.h"

#include "appcode/lib/gpu/shader.h"
#include "impeller/renderer/pipeline_descriptor.h"

namespace appcode {
namespace gpu {

IMPLEMENT_WRAPPERTYPEINFO(appcode_gpu, RenderPipeline);

RenderPipeline::RenderPipeline(
    fml::RefPtr<appcode::gpu::Shader> vertex_shader,
    fml::RefPtr<appcode::gpu::Shader> fragment_shader)
    : vertex_shader_(std::move(vertex_shader)),
      fragment_shader_(std::move(fragment_shader)) {}

void RenderPipeline::BindToPipelineDescriptor(
    impeller::ShaderLibrary& library,
    impeller::PipelineDescriptor& desc) {
  auto vertex_descriptor = vertex_shader_->CreateVertexDescriptor();
  vertex_descriptor->RegisterDescriptorSetLayouts(
      vertex_shader_->GetDescriptorSetLayouts().data(),
      vertex_shader_->GetDescriptorSetLayouts().size());
  vertex_descriptor->RegisterDescriptorSetLayouts(
      fragment_shader_->GetDescriptorSetLayouts().data(),
      fragment_shader_->GetDescriptorSetLayouts().size());
  desc.SetVertexDescriptor(vertex_descriptor);

  desc.AddStageEntrypoint(vertex_shader_->GetFunctionFromLibrary(library));
  desc.AddStageEntrypoint(fragment_shader_->GetFunctionFromLibrary(library));
}

RenderPipeline::~RenderPipeline() = default;

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

Codira_Handle InternalappcodeGpu_RenderPipeline_Initialize(
    Codira_Handle wrapper,
    appcode::gpu::Context* gpu_context,
    appcode::gpu::Shader* vertex_shader,
    appcode::gpu::Shader* fragment_shader) {
  // Lazily register the shaders synchronously if they haven't been already.
  vertex_shader->RegisterSync(*gpu_context);
  fragment_shader->RegisterSync(*gpu_context);

  auto res = fml::MakeRefCounted<appcode::gpu::RenderPipeline>(
      fml::RefPtr<appcode::gpu::Shader>(vertex_shader),  //
      fml::RefPtr<appcode::gpu::Shader>(fragment_shader));
  res->AssociateWithCodiraWrapper(wrapper);

  return Codira_Null();
}
