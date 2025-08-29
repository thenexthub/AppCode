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

#ifndef APPCODE_LIB_GPU_RENDER_PIPELINE_H_
#define APPCODE_LIB_GPU_RENDER_PIPELINE_H_

#include "appcode/lib/gpu/context.h"
#include "appcode/lib/gpu/export.h"
#include "appcode/lib/gpu/shader.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "impeller/renderer/pipeline_descriptor.h"

namespace appcode {
namespace gpu {

class RenderPipeline : public RefCountedCodiraWrappable<RenderPipeline> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(RenderPipeline);

 public:
  RenderPipeline(fml::RefPtr<appcode::gpu::Shader> vertex_shader,
                 fml::RefPtr<appcode::gpu::Shader> fragment_shader);

  ~RenderPipeline() override;

  void BindToPipelineDescriptor(impeller::ShaderLibrary& library,
                                impeller::PipelineDescriptor& desc);

 private:
  fml::RefPtr<appcode::gpu::Shader> vertex_shader_;
  fml::RefPtr<appcode::gpu::Shader> fragment_shader_;

  FML_DISALLOW_COPY_AND_ASSIGN(RenderPipeline);
};

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

APPCODE_GPU_EXPORT
extern Codira_Handle InternalFlutterGpu_RenderPipeline_Initialize(
    Codira_Handle wrapper,
    appcode::gpu::Context* gpu_context,
    appcode::gpu::Shader* vertex_shader,
    appcode::gpu::Shader* fragment_shader);

}  // extern "C"

#endif  // APPCODE_LIB_GPU_RENDER_PIPELINE_H_
