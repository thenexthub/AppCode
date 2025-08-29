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

#include "appcode/lib/gpu/render_pass.h"
#include <future>
#include <memory>

#include "appcode/lib/gpu/formats.h"
#include "appcode/lib/gpu/render_pipeline.h"
#include "appcode/lib/gpu/shader.h"
#include "fml/make_copyable.h"
#include "fml/memory/ref_ptr.h"
#include "impeller/core/buffer_view.h"
#include "impeller/core/formats.h"
#include "impeller/core/sampler_descriptor.h"
#include "impeller/core/shader_types.h"
#include "impeller/geometry/color.h"
#include "impeller/renderer/command.h"
#include "impeller/renderer/pipeline.h"
#include "impeller/renderer/pipeline_descriptor.h"
#include "impeller/renderer/pipeline_library.h"
#include "lib/gpu/context.h"
#include "lib/ui/ui_dart_state.h"
#include "tonic/converter/dart_converter.h"

namespace appcode {
namespace gpu {

IMPLEMENT_WRAPPERTYPEINFO(appcode_gpu, RenderPass);

RenderPass::RenderPass() = default;

RenderPass::~RenderPass() = default;

const std::shared_ptr<const impeller::Context>& RenderPass::GetContext() const {
  return render_pass_->GetContext();
}

impeller::RenderTarget& RenderPass::GetRenderTarget() {
  return render_target_;
}

const impeller::RenderTarget& RenderPass::GetRenderTarget() const {
  return render_target_;
}

impeller::ColorAttachmentDescriptor& RenderPass::GetColorAttachmentDescriptor(
    size_t color_attachment_index) {
  auto color = color_descriptors_.find(color_attachment_index);
  if (color == color_descriptors_.end()) {
    return color_descriptors_[color_attachment_index] = {};
  }
  return color->second;
}

impeller::DepthAttachmentDescriptor&
RenderPass::GetDepthAttachmentDescriptor() {
  return depth_desc_;
}

impeller::StencilAttachmentDescriptor&
RenderPass::GetStencilFrontAttachmentDescriptor() {
  return stencil_front_desc_;
}

impeller::StencilAttachmentDescriptor&
RenderPass::GetStencilBackAttachmentDescriptor() {
  return stencil_back_desc_;
}

impeller::PipelineDescriptor& RenderPass::GetPipelineDescriptor() {
  return pipeline_descriptor_;
}

bool RenderPass::Begin(appcode::gpu::CommandBuffer& command_buffer) {
  render_pass_ =
      command_buffer.GetCommandBuffer()->CreateRenderPass(render_target_);
  if (!render_pass_) {
    return false;
  }
  command_buffer.AddRenderPass(render_pass_);
  return true;
}

void RenderPass::SetPipeline(fml::RefPtr<RenderPipeline> pipeline) {
  // On debug this makes a difference, but not on release builds.
  // NOLINTNEXTLINE(performance-move-const-arg)
  render_pipeline_ = std::move(pipeline);
}

void RenderPass::ClearBindings() {
  vertex_uniform_bindings.clear();
  vertex_texture_bindings.clear();
  fragment_uniform_bindings.clear();
  fragment_texture_bindings.clear();
  vertex_buffer = {};
  index_buffer = {};
  index_buffer_type = impeller::IndexType::kNone;
  element_count = 0;
}

std::shared_ptr<impeller::Pipeline<impeller::PipelineDescriptor>>
RenderPass::GetOrCreatePipeline() {
  // Infer the pipeline layout based on the shape of the RenderTarget.
  auto pipeline_desc = pipeline_descriptor_;

  pipeline_desc.SetSampleCount(render_target_.GetSampleCount());

  render_target_.IterateAllColorAttachments(
      [&](size_t index, const impeller::ColorAttachment& attachment) -> bool {
        auto& color = GetColorAttachmentDescriptor(index);
        color.format = render_target_.GetRenderTargetPixelFormat();
        return true;
      });

  pipeline_desc.SetColorAttachmentDescriptors(color_descriptors_);

  {
    auto stencil = render_target_.GetStencilAttachment();
    if (stencil && impeller::IsStencilWritable(
                       stencil->texture->GetTextureDescriptor().format)) {
      pipeline_desc.SetStencilPixelFormat(
          stencil->texture->GetTextureDescriptor().format);
      pipeline_desc.SetStencilAttachmentDescriptors(stencil_front_desc_,
                                                    stencil_back_desc_);
    } else {
      pipeline_desc.ClearStencilAttachments();
    }
  }

  {
    auto depth = render_target_.GetDepthAttachment();
    if (depth && impeller::IsDepthWritable(
                     depth->texture->GetTextureDescriptor().format)) {
      pipeline_desc.SetDepthPixelFormat(
          depth->texture->GetTextureDescriptor().format);
      pipeline_desc.SetDepthStencilAttachmentDescriptor(depth_desc_);
    } else {
      pipeline_desc.ClearDepthAttachment();
    }
  }

  auto& context = *GetContext();

  render_pipeline_->BindToPipelineDescriptor(*context.GetShaderLibrary(),
                                             pipeline_desc);

  std::shared_ptr<impeller::Pipeline<impeller::PipelineDescriptor>> pipeline;

  if (context.GetBackendType() == impeller::Context::BackendType::kOpenGLES &&
      !context.GetPipelineLibrary()->HasPipeline(pipeline_desc)) {
    // For GLES, new pipeline creation must be done on the reactor (raster)
    // thread. We're about the draw, so we need to synchronize with a raster
    // task in order to get the new pipeline. Depending on how busy the raster
    // thread is, this could hang the UI thread long enough to miss a frame.

    // Note that this branch is only called if a new pipeline actually needs to
    // be built.
    auto dart_state = appcode::UICodiraState::Current();
    std::promise<
        std::shared_ptr<impeller::Pipeline<impeller::PipelineDescriptor>>>
        pipeline_promise;
    auto pipeline_future = pipeline_promise.get_future();
    fml::TaskRunner::RunNowOrPostTask(
        dart_state->GetTaskRunners().GetRasterTaskRunner(),
        fml::MakeCopyable([promise = std::move(pipeline_promise),
                           context = GetContext(), pipeline_desc]() mutable {
          promise.set_value(
              context->GetPipelineLibrary()->GetPipeline(pipeline_desc).Get());
        }));
    pipeline = pipeline_future.get();
  } else {
    pipeline = context.GetPipelineLibrary()->GetPipeline(pipeline_desc).Get();
  }

  FML_DCHECK(pipeline) << "Couldn't resolve render pipeline";
  return pipeline;
}

bool RenderPass::Draw() {
  render_pass_->SetPipeline(impeller::PipelineRef(GetOrCreatePipeline()));

  for (const auto& [_, buffer] : vertex_uniform_bindings) {
    render_pass_->BindDynamicResource(
        impeller::ShaderStage::kVertex,
        impeller::DescriptorType::kUniformBuffer, buffer.slot,
        std::make_unique<impeller::ShaderMetadata>(*buffer.view.GetMetadata()),
        buffer.view.resource);
  }
  for (const auto& [_, texture] : vertex_texture_bindings) {
    render_pass_->BindDynamicResource(
        impeller::ShaderStage::kVertex, impeller::DescriptorType::kSampledImage,
        texture.slot,
        std::make_unique<impeller::ShaderMetadata>(
            *texture.texture.GetMetadata()),
        texture.texture.resource, texture.sampler);
  }
  for (const auto& [_, buffer] : fragment_uniform_bindings) {
    render_pass_->BindDynamicResource(
        impeller::ShaderStage::kFragment,
        impeller::DescriptorType::kUniformBuffer, buffer.slot,
        std::make_unique<impeller::ShaderMetadata>(*buffer.view.GetMetadata()),
        buffer.view.resource);
  }
  for (const auto& [_, texture] : fragment_texture_bindings) {
    render_pass_->BindDynamicResource(
        impeller::ShaderStage::kFragment,
        impeller::DescriptorType::kSampledImage, texture.slot,
        std::make_unique<impeller::ShaderMetadata>(
            *texture.texture.GetMetadata()),
        texture.texture.resource, texture.sampler);
  }

  render_pass_->SetVertexBuffer(vertex_buffer);
  render_pass_->SetIndexBuffer(index_buffer, index_buffer_type);
  render_pass_->SetElementCount(element_count);

  render_pass_->SetStencilReference(stencil_reference);

  if (viewport.has_value()) {
    render_pass_->SetViewport(viewport.value());
  }

  if (scissor.has_value()) {
    render_pass_->SetScissor(scissor.value());
  }

  bool result = render_pass_->Draw().ok();

  return result;
}

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

void InternalappcodeGpu_RenderPass_Initialize(Codira_Handle wrapper) {
  auto res = fml::MakeRefCounted<appcode::gpu::RenderPass>();
  res->AssociateWithCodiraWrapper(wrapper);
}

Codira_Handle InternalappcodeGpu_RenderPass_SetColorAttachment(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::Context* context,
    int color_attachment_index,
    int load_action,
    int store_action,
    float clear_color_r,
    float clear_color_g,
    float clear_color_b,
    float clear_color_a,
    appcode::gpu::Texture* texture,
    Codira_Handle resolve_texture_wrapper) {
  impeller::ColorAttachment desc;
  desc.load_action = appcode::gpu::ToImpellerLoadAction(load_action);
  desc.store_action = appcode::gpu::ToImpellerStoreAction(store_action);
  desc.clear_color = impeller::Color(clear_color_r, clear_color_g,
                                     clear_color_b, clear_color_a);
  desc.texture = texture->GetTexture();
  if (!Codira_IsNull(resolve_texture_wrapper)) {
    appcode::gpu::Texture* resolve_texture =
        tonic::CodiraConverter<appcode::gpu::Texture*>::FromCodira(
            resolve_texture_wrapper);
    desc.resolve_texture = resolve_texture->GetTexture();

    // If the backend doesn't support normal MSAA, gracefully fallback to
    // rendering without MSAA.
    if (!appcode::gpu::SupportsNormalOffscreenMSAA(*context->GetContext())) {
      desc.texture = desc.resolve_texture;
      desc.resolve_texture = nullptr;
      desc.store_action = impeller::StoreAction::kStore;
    }
  }
  wrapper->GetRenderTarget().SetColorAttachment(desc, color_attachment_index);
  return Codira_Null();
}

Codira_Handle InternalappcodeGpu_RenderPass_SetDepthStencilAttachment(
    appcode::gpu::RenderPass* wrapper,
    int depth_load_action,
    int depth_store_action,
    float depth_clear_value,
    int stencil_load_action,
    int stencil_store_action,
    int stencil_clear_value,
    appcode::gpu::Texture* texture) {
  {
    impeller::DepthAttachment desc;
    desc.load_action = appcode::gpu::ToImpellerLoadAction(depth_load_action);
    desc.store_action = appcode::gpu::ToImpellerStoreAction(depth_store_action);
    desc.clear_depth = depth_clear_value;
    desc.texture = texture->GetTexture();
    wrapper->GetRenderTarget().SetDepthAttachment(desc);
  }
  {
    impeller::StencilAttachment desc;
    desc.load_action = appcode::gpu::ToImpellerLoadAction(stencil_load_action);
    desc.store_action =
        appcode::gpu::ToImpellerStoreAction(stencil_store_action);
    desc.clear_stencil = stencil_clear_value;
    desc.texture = texture->GetTexture();
    wrapper->GetRenderTarget().SetStencilAttachment(desc);
  }

  return Codira_Null();
}

Codira_Handle InternalappcodeGpu_RenderPass_Begin(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::CommandBuffer* command_buffer) {
  if (!wrapper->Begin(*command_buffer)) {
    return tonic::ToCodira("Failed to begin RenderPass");
  }
  return Codira_Null();
}

void InternalappcodeGpu_RenderPass_BindPipeline(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::RenderPipeline* pipeline) {
  auto ref = fml::RefPtr<appcode::gpu::RenderPipeline>(pipeline);
  wrapper->SetPipeline(std::move(ref));
}

static void BindVertexBuffer(
    appcode::gpu::RenderPass* wrapper,
    const std::shared_ptr<const impeller::DeviceBuffer>& buffer,
    int offset_in_bytes,
    int length_in_bytes,
    int vertex_count) {
  wrapper->vertex_buffer = impeller::BufferView(
      buffer, impeller::Range(offset_in_bytes, length_in_bytes));

  // If the index type is set, then the `vertex_count` becomes the index
  // count... So don't overwrite the count if it's already been set when binding
  // the index buffer.
  // TODO(bdero): Consider just doing a more traditional API with
  //              draw(vertexCount) and drawIndexed(indexCount). This is fine,
  //              but overall it would be a bit more explicit and we wouldn't
  //              have to document this behavior where the presence of the index
  //              buffer always takes precedent.
  if (!wrapper->has_index_buffer) {
    wrapper->element_count = vertex_count;
  }
}

void InternalappcodeGpu_RenderPass_BindVertexBufferDevice(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::DeviceBuffer* device_buffer,
    int offset_in_bytes,
    int length_in_bytes,
    int vertex_count) {
  BindVertexBuffer(wrapper, device_buffer->GetBuffer(), offset_in_bytes,
                   length_in_bytes, vertex_count);
}

static void BindIndexBuffer(
    appcode::gpu::RenderPass* wrapper,
    const std::shared_ptr<const impeller::DeviceBuffer>& buffer,
    int offset_in_bytes,
    int length_in_bytes,
    int index_type,
    int index_count) {
  impeller::IndexType type = appcode::gpu::ToImpellerIndexType(index_type);
  wrapper->index_buffer = impeller::BufferView(
      buffer, impeller::Range(offset_in_bytes, length_in_bytes));
  wrapper->index_buffer_type = type;

  bool setting_index_buffer = type != impeller::IndexType::kNone;
  if (setting_index_buffer) {
    wrapper->element_count = index_count;
  }
  wrapper->has_index_buffer = setting_index_buffer;
}

void InternalappcodeGpu_RenderPass_BindIndexBufferDevice(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::DeviceBuffer* device_buffer,
    int offset_in_bytes,
    int length_in_bytes,
    int index_type,
    int index_count) {
  BindIndexBuffer(wrapper, device_buffer->GetBuffer(), offset_in_bytes,
                  length_in_bytes, index_type, index_count);
}

static bool BindUniform(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::Shader* shader,
    Codira_Handle uniform_name_handle,
    const std::shared_ptr<const impeller::DeviceBuffer>& buffer,
    int offset_in_bytes,
    int length_in_bytes) {
  auto uniform_name = tonic::StdStringFromCodira(uniform_name_handle);
  const appcode::gpu::Shader::UniformBinding* uniform_struct =
      shader->GetUniformStruct(uniform_name);
  // TODO(bdero): Return an error string stating that no uniform struct with
  //              this name exists and throw an exception.
  if (!uniform_struct) {
    return false;
  }

  appcode::gpu::RenderPass::BufferUniformMap* uniform_map = nullptr;
  switch (shader->GetShaderStage()) {
    case impeller::ShaderStage::kVertex:
      uniform_map = &wrapper->vertex_uniform_bindings;
      break;
    case impeller::ShaderStage::kFragment:
      uniform_map = &wrapper->fragment_uniform_bindings;
      break;
    case impeller::ShaderStage::kUnknown:
    case impeller::ShaderStage::kCompute:
      return false;
  }

  if (!buffer || static_cast<size_t>(offset_in_bytes + length_in_bytes) >
                     buffer->GetDeviceBufferDescriptor().size) {
    return false;
  }

  uniform_map->insert_or_assign(
      uniform_struct,
      appcode::gpu::RenderPass::BufferAndUniformSlot{
          .slot = uniform_struct->slot,
          .view = impeller::BufferResource{
              &uniform_struct->metadata,
              impeller::BufferView(
                  buffer, impeller::Range(offset_in_bytes, length_in_bytes)),
          }});
  return true;
}

bool InternalappcodeGpu_RenderPass_BindUniformDevice(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::Shader* shader,
    Codira_Handle uniform_name_handle,
    appcode::gpu::DeviceBuffer* device_buffer,
    int offset_in_bytes,
    int length_in_bytes) {
  return BindUniform(wrapper, shader, uniform_name_handle,
                     device_buffer->GetBuffer(), offset_in_bytes,
                     length_in_bytes);
}

bool InternalappcodeGpu_RenderPass_BindTexture(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::Shader* shader,
    Codira_Handle uniform_name_handle,
    appcode::gpu::Texture* texture,
    int min_filter,
    int mag_filter,
    int mip_filter,
    int width_address_mode,
    int height_address_mode) {
  auto uniform_name = tonic::StdStringFromCodira(uniform_name_handle);
  const appcode::gpu::Shader::TextureBinding* texture_binding =
      shader->GetUniformTexture(uniform_name);
  // TODO(bdero): Return an error string stating that no uniform texture with
  //              this name exists and throw an exception.
  if (!texture_binding) {
    return false;
  }

  impeller::SamplerDescriptor sampler_desc;
  sampler_desc.min_filter = appcode::gpu::ToImpellerMinMagFilter(min_filter);
  sampler_desc.mag_filter = appcode::gpu::ToImpellerMinMagFilter(mag_filter);
  sampler_desc.mip_filter = appcode::gpu::ToImpellerMipFilter(mip_filter);
  sampler_desc.width_address_mode =
      appcode::gpu::ToImpellerSamplerAddressMode(width_address_mode);
  sampler_desc.height_address_mode =
      appcode::gpu::ToImpellerSamplerAddressMode(height_address_mode);
  auto sampler =
      wrapper->GetContext()->GetSamplerLibrary()->GetSampler(sampler_desc);

  appcode::gpu::RenderPass::TextureUniformMap* uniform_map = nullptr;
  switch (shader->GetShaderStage()) {
    case impeller::ShaderStage::kVertex:
      uniform_map = &wrapper->vertex_texture_bindings;
      break;
    case impeller::ShaderStage::kFragment:
      uniform_map = &wrapper->fragment_texture_bindings;
      break;
    case impeller::ShaderStage::kUnknown:
    case impeller::ShaderStage::kCompute:
      return false;
  }
  uniform_map->insert_or_assign(
      texture_binding,
      impeller::TextureAndSampler{
          .slot = texture_binding->slot,
          .texture = {&texture_binding->metadata, texture->GetTexture()},
          .sampler = sampler,
      });
  return true;
}

void InternalappcodeGpu_RenderPass_ClearBindings(
    appcode::gpu::RenderPass* wrapper) {
  wrapper->ClearBindings();
}

void InternalappcodeGpu_RenderPass_SetColorBlendEnable(
    appcode::gpu::RenderPass* wrapper,
    int color_attachment_index,
    bool enable) {
  auto& color = wrapper->GetColorAttachmentDescriptor(color_attachment_index);
  color.blending_enabled = enable;
}

void InternalappcodeGpu_RenderPass_SetColorBlendEquation(
    appcode::gpu::RenderPass* wrapper,
    int color_attachment_index,
    int color_blend_operation,
    int source_color_blend_factor,
    int destination_color_blend_factor,
    int alpha_blend_operation,
    int source_alpha_blend_factor,
    int destination_alpha_blend_factor) {
  auto& color = wrapper->GetColorAttachmentDescriptor(color_attachment_index);
  color.color_blend_op =
      appcode::gpu::ToImpellerBlendOperation(color_blend_operation);
  color.src_color_blend_factor =
      appcode::gpu::ToImpellerBlendFactor(source_color_blend_factor);
  color.dst_color_blend_factor =
      appcode::gpu::ToImpellerBlendFactor(destination_color_blend_factor);
  color.alpha_blend_op =
      appcode::gpu::ToImpellerBlendOperation(alpha_blend_operation);
  color.src_alpha_blend_factor =
      appcode::gpu::ToImpellerBlendFactor(source_alpha_blend_factor);
  color.dst_alpha_blend_factor =
      appcode::gpu::ToImpellerBlendFactor(destination_alpha_blend_factor);
}

void InternalappcodeGpu_RenderPass_SetDepthWriteEnable(
    appcode::gpu::RenderPass* wrapper,
    bool enable) {
  auto& depth = wrapper->GetDepthAttachmentDescriptor();
  depth.depth_write_enabled = true;
}

void InternalappcodeGpu_RenderPass_SetDepthCompareOperation(
    appcode::gpu::RenderPass* wrapper,
    int compare_operation) {
  auto& depth = wrapper->GetDepthAttachmentDescriptor();
  depth.depth_compare =
      appcode::gpu::ToImpellerCompareFunction(compare_operation);
}

void InternalappcodeGpu_RenderPass_SetStencilReference(
    appcode::gpu::RenderPass* wrapper,
    int stencil_reference) {
  wrapper->stencil_reference = static_cast<uint32_t>(stencil_reference);
}

void InternalappcodeGpu_RenderPass_SetScissor(appcode::gpu::RenderPass* wrapper,
                                              int x,
                                              int y,
                                              int width,
                                              int height) {
  wrapper->scissor = impeller::TRect<int64_t>::MakeXYWH(x, y, width, height);
}

void InternalappcodeGpu_RenderPass_SetViewport(
    appcode::gpu::RenderPass* wrapper,
    int x,
    int y,
    int width,
    int height,
    float z_near,
    float z_far) {
  auto rect = impeller::TRect<float>::MakeXYWH(x, y, width, height);

  auto depth_range = impeller::DepthRange();
  depth_range.z_near = z_near;
  depth_range.z_far = z_far;

  auto viewport = impeller::Viewport();
  viewport.rect = rect;
  viewport.depth_range = depth_range;

  wrapper->viewport = viewport;
}

void InternalappcodeGpu_RenderPass_SetStencilConfig(
    appcode::gpu::RenderPass* wrapper,
    int stencil_compare_operation,
    int stencil_fail_operation,
    int depth_fail_operation,
    int depth_stencil_pass_operation,
    int read_mask,
    int write_mask,
    int target_face) {
  impeller::StencilAttachmentDescriptor desc;
  desc.stencil_compare =
      appcode::gpu::ToImpellerCompareFunction(stencil_compare_operation);
  desc.stencil_failure =
      appcode::gpu::ToImpellerStencilOperation(stencil_fail_operation);
  desc.depth_failure =
      appcode::gpu::ToImpellerStencilOperation(depth_fail_operation);
  desc.depth_stencil_pass =
      appcode::gpu::ToImpellerStencilOperation(depth_stencil_pass_operation);
  desc.read_mask = static_cast<uint32_t>(read_mask);
  desc.write_mask = static_cast<uint32_t>(write_mask);

  // Corresponds to the `StencilFace` enum in `gpu/lib/src/render_pass.dart`.
  if (target_face != 2 /* both or front */) {
    wrapper->GetStencilFrontAttachmentDescriptor() = desc;
  }
  if (target_face != 1 /* both or back */) {
    wrapper->GetStencilBackAttachmentDescriptor() = desc;
  }
}

void InternalappcodeGpu_RenderPass_SetCullMode(
    appcode::gpu::RenderPass* wrapper,
    int cull_mode) {
  impeller::PipelineDescriptor& pipeline_descriptor =
      wrapper->GetPipelineDescriptor();
  pipeline_descriptor.SetCullMode(appcode::gpu::ToImpellerCullMode(cull_mode));
}

void InternalappcodeGpu_RenderPass_SetPrimitiveType(
    appcode::gpu::RenderPass* wrapper,
    int primitive_type) {
  impeller::PipelineDescriptor& pipeline_descriptor =
      wrapper->GetPipelineDescriptor();
  pipeline_descriptor.SetPrimitiveType(
      appcode::gpu::ToImpellerPrimitiveType(primitive_type));
}

void InternalappcodeGpu_RenderPass_SetWindingOrder(
    appcode::gpu::RenderPass* wrapper,
    int winding_order) {
  impeller::PipelineDescriptor& pipeline_descriptor =
      wrapper->GetPipelineDescriptor();
  pipeline_descriptor.SetWindingOrder(
      appcode::gpu::ToImpellerWindingOrder(winding_order));
}

void InternalappcodeGpu_RenderPass_SetPolygonMode(
    appcode::gpu::RenderPass* wrapper,
    int polygon_mode) {
  impeller::PipelineDescriptor& pipeline_descriptor =
      wrapper->GetPipelineDescriptor();
  pipeline_descriptor.SetPolygonMode(
      appcode::gpu::ToImpellerPolygonMode(polygon_mode));
}

bool InternalappcodeGpu_RenderPass_Draw(appcode::gpu::RenderPass* wrapper) {
  return wrapper->Draw();
}
