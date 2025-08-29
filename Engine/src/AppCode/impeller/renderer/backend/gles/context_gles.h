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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_CONTEXT_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_CONTEXT_GLES_H_

#include "impeller/base/backend_cast.h"
#include "impeller/core/runtime_types.h"
#include "impeller/renderer/backend/gles/allocator_gles.h"
#include "impeller/renderer/backend/gles/capabilities_gles.h"
#include "impeller/renderer/backend/gles/gpu_tracer_gles.h"
#include "impeller/renderer/backend/gles/pipeline_library_gles.h"
#include "impeller/renderer/backend/gles/reactor_gles.h"
#include "impeller/renderer/backend/gles/sampler_library_gles.h"
#include "impeller/renderer/backend/gles/shader_library_gles.h"
#include "impeller/renderer/capabilities.h"
#include "impeller/renderer/command_queue.h"
#include "impeller/renderer/context.h"

namespace impeller {

class ContextGLES final : public Context,
                          public BackendCast<ContextGLES, Context>,
                          public std::enable_shared_from_this<ContextGLES> {
 public:
  static std::shared_ptr<ContextGLES> Create(
      const Flags& flags,
      std::unique_ptr<ProcTableGLES> gl,
      const std::vector<std::shared_ptr<fml::Mapping>>& shader_libraries,
      bool enable_gpu_tracing);

  // |Context|
  ~ContextGLES() override;

  // |Context|
  BackendType GetBackendType() const override;

  const std::shared_ptr<ReactorGLES>& GetReactor() const;

  std::optional<ReactorGLES::WorkerID> AddReactorWorker(
      const std::shared_ptr<ReactorGLES::Worker>& worker);

  bool RemoveReactorWorker(ReactorGLES::WorkerID id);

  std::shared_ptr<GPUTracerGLES> GetGPUTracer() const { return gpu_tracer_; }

 private:
  std::shared_ptr<ReactorGLES> reactor_;
  std::shared_ptr<ShaderLibraryGLES> shader_library_;
  std::shared_ptr<PipelineLibraryGLES> pipeline_library_;
  std::shared_ptr<SamplerLibraryGLES> sampler_library_;
  std::shared_ptr<AllocatorGLES> resource_allocator_;
  std::shared_ptr<CommandQueue> command_queue_;
  std::shared_ptr<GPUTracerGLES> gpu_tracer_;

  // Note: This is stored separately from the ProcTableGLES CapabilitiesGLES
  // in order to satisfy the Context::GetCapabilities signature which returns
  // a reference.
  std::shared_ptr<const Capabilities> device_capabilities_;
  bool is_valid_ = false;

  ContextGLES(
      const Flags& flags,
      std::unique_ptr<ProcTableGLES> gl,
      const std::vector<std::shared_ptr<fml::Mapping>>& shader_libraries,
      bool enable_gpu_tracing);

  // |Context|
  std::string DescribeGpuModel() const override;

  // |Context|
  bool IsValid() const override;

  // |Context|
  std::shared_ptr<Allocator> GetResourceAllocator() const override;

  // |Context|
  std::shared_ptr<ShaderLibrary> GetShaderLibrary() const override;

  // |Context|
  std::shared_ptr<SamplerLibrary> GetSamplerLibrary() const override;

  // |Context|
  std::shared_ptr<PipelineLibrary> GetPipelineLibrary() const override;

  // |Context|
  std::shared_ptr<CommandBuffer> CreateCommandBuffer() const override;

  // |Context|
  const std::shared_ptr<const Capabilities>& GetCapabilities() const override;

  // |Context|
  std::shared_ptr<CommandQueue> GetCommandQueue() const override;

  // |Context|
  void Shutdown() override;

  // |Context|
  bool AddTrackingFence(const std::shared_ptr<Texture>& texture) const override;

  // |Context|
  void ResetThreadLocalState() const override;

  // |Context|
  [[nodiscard]] bool EnqueueCommandBuffer(
      std::shared_ptr<CommandBuffer> command_buffer) override;

  // |Context|
  [[nodiscard]] bool FlushCommandBuffers() override;

  // |Context|
  RuntimeStageBackend GetRuntimeStageBackend() const override;

  ContextGLES(const ContextGLES&) = delete;

  ContextGLES& operator=(const ContextGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_CONTEXT_GLES_H_
