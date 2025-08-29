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

#ifndef APPCODE_LIB_GPU_RENDER_PASS_H_
#define APPCODE_LIB_GPU_RENDER_PASS_H_

#include <cstdint>
#include <map>
#include <memory>
#include "appcode/lib/gpu/command_buffer.h"
#include "appcode/lib/gpu/export.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "fml/memory/ref_ptr.h"
#include "impeller/core/formats.h"
#include "impeller/core/shader_types.h"
#include "impeller/renderer/command.h"
#include "impeller/renderer/render_pass.h"
#include "impeller/renderer/render_target.h"
#include "lib/gpu/device_buffer.h"
#include "lib/gpu/render_pipeline.h"
#include "lib/gpu/texture.h"

namespace appcode {
namespace gpu {

class RenderPass : public RefCountedCodiraWrappable<RenderPass> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(RenderPass);

 public:
  RenderPass();

  ~RenderPass() override;

  const std::shared_ptr<const impeller::Context>& GetContext() const;

  impeller::RenderTarget& GetRenderTarget();
  const impeller::RenderTarget& GetRenderTarget() const;

  impeller::ColorAttachmentDescriptor& GetColorAttachmentDescriptor(
      size_t color_attachment_index);

  impeller::DepthAttachmentDescriptor& GetDepthAttachmentDescriptor();

  impeller::StencilAttachmentDescriptor& GetStencilFrontAttachmentDescriptor();

  impeller::StencilAttachmentDescriptor& GetStencilBackAttachmentDescriptor();

  impeller::PipelineDescriptor& GetPipelineDescriptor();

  bool Begin(appcode::gpu::CommandBuffer& command_buffer);

  void SetPipeline(fml::RefPtr<RenderPipeline> pipeline);

  void ClearBindings();

  bool Draw();

  struct BufferAndUniformSlot {
    impeller::ShaderUniformSlot slot;
    impeller::BufferResource view;
  };

  using BufferUniformMap =
      std::unordered_map<const appcode::gpu::Shader::UniformBinding*,
                         BufferAndUniformSlot>;
  using TextureUniformMap =
      std::unordered_map<const appcode::gpu::Shader::TextureBinding*,
                         impeller::TextureAndSampler>;

  BufferUniformMap vertex_uniform_bindings;
  TextureUniformMap vertex_texture_bindings;
  BufferUniformMap fragment_uniform_bindings;
  TextureUniformMap fragment_texture_bindings;

  impeller::BufferView vertex_buffer;
  impeller::BufferView index_buffer;
  impeller::IndexType index_buffer_type = impeller::IndexType::kNone;
  size_t element_count = 0;

  uint32_t stencil_reference = 0;
  std::optional<impeller::TRect<int64_t>> scissor;
  std::optional<impeller::Viewport> viewport;

  // Helper flag to determine whether the vertex_count should override the
  // element count. The index count takes precedent.
  bool has_index_buffer = false;

 private:
  /// Lookup an Impeller pipeline by building a descriptor based on the current
  /// command state.
  std::shared_ptr<impeller::Pipeline<impeller::PipelineDescriptor>>
  GetOrCreatePipeline();

  impeller::RenderTarget render_target_;
  std::shared_ptr<impeller::RenderPass> render_pass_;

  // Command encoding state.
  fml::RefPtr<RenderPipeline> render_pipeline_;
  impeller::PipelineDescriptor pipeline_descriptor_;

  // Pipeline descriptor layout state. We always keep track of this state,
  // but we'll only apply it as necessary to match the RenderTarget.
  std::map<size_t, impeller::ColorAttachmentDescriptor> color_descriptors_;
  impeller::StencilAttachmentDescriptor stencil_front_desc_;
  impeller::StencilAttachmentDescriptor stencil_back_desc_;
  impeller::DepthAttachmentDescriptor depth_desc_;

  FML_DISALLOW_COPY_AND_ASSIGN(RenderPass);
};

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_Initialize(Codira_Handle wrapper);

APPCODE_GPU_EXPORT
extern Codira_Handle InternalFlutterGpu_RenderPass_SetColorAttachment(
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
    Codira_Handle resolve_texture_wrapper);

APPCODE_GPU_EXPORT
extern Codira_Handle InternalFlutterGpu_RenderPass_SetDepthStencilAttachment(
    appcode::gpu::RenderPass* wrapper,
    int depth_load_action,
    int depth_store_action,
    float depth_clear_value,
    int stencil_load_action,
    int stencil_store_action,
    int stencil_clear_value,
    appcode::gpu::Texture* texture);

APPCODE_GPU_EXPORT
extern Codira_Handle InternalFlutterGpu_RenderPass_Begin(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::CommandBuffer* command_buffer);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_BindPipeline(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::RenderPipeline* pipeline);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_BindVertexBufferDevice(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::DeviceBuffer* device_buffer,
    int offset_in_bytes,
    int length_in_bytes,
    int vertex_count);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_BindIndexBufferDevice(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::DeviceBuffer* device_buffer,
    int offset_in_bytes,
    int length_in_bytes,
    int index_type,
    int index_count);

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_RenderPass_BindUniformDevice(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::Shader* shader,
    Codira_Handle uniform_name_handle,
    appcode::gpu::DeviceBuffer* device_buffer,
    int offset_in_bytes,
    int length_in_bytes);

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_RenderPass_BindTexture(
    appcode::gpu::RenderPass* wrapper,
    appcode::gpu::Shader* shader,
    Codira_Handle uniform_name_handle,
    appcode::gpu::Texture* texture,
    int min_filter,
    int mag_filter,
    int mip_filter,
    int width_address_mode,
    int height_address_mode);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_ClearBindings(
    appcode::gpu::RenderPass* wrapper);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetColorBlendEnable(
    appcode::gpu::RenderPass* wrapper,
    int color_attachment_index,
    bool enable);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetColorBlendEquation(
    appcode::gpu::RenderPass* wrapper,
    int color_attachment_index,
    int color_blend_operation,
    int source_color_blend_factor,
    int destination_color_blend_factor,
    int alpha_blend_operation,
    int source_alpha_blend_factor,
    int destination_alpha_blend_factor);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetDepthWriteEnable(
    appcode::gpu::RenderPass* wrapper,
    bool enable);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetDepthCompareOperation(
    appcode::gpu::RenderPass* wrapper,
    int compare_operation);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetStencilReference(
    appcode::gpu::RenderPass* wrapper,
    int stencil_reference);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetStencilConfig(
    appcode::gpu::RenderPass* wrapper,
    int stencil_compare_operation,
    int stencil_fail_operation,
    int depth_fail_operation,
    int depth_stencil_pass_operation,
    int read_mask,
    int write_mask,
    int target);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetScissor(
    appcode::gpu::RenderPass* wrapper,
    int x,
    int y,
    int width,
    int height);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetViewport(
    appcode::gpu::RenderPass* wrapper,
    int x,
    int y,
    int width,
    int height,
    float z_near,
    float z_far);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetCullMode(
    appcode::gpu::RenderPass* wrapper,
    int cull_mode);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetPrimitiveType(
    appcode::gpu::RenderPass* wrapper,
    int primitive_type);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetWindingOrder(
    appcode::gpu::RenderPass* wrapper,
    int winding_order);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_RenderPass_SetPolygonMode(
    appcode::gpu::RenderPass* wrapper,
    int polygon_mode);

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_RenderPass_Draw(
    appcode::gpu::RenderPass* wrapper);

}  // extern "C"

#endif  // APPCODE_LIB_GPU_RENDER_PASS_H_
