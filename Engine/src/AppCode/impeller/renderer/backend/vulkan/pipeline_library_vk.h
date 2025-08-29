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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_PIPELINE_LIBRARY_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_PIPELINE_LIBRARY_VK_H_

#include <atomic>

#include "appcode/fml/concurrent_message_loop.h"
#include "appcode/fml/unique_fd.h"
#include "impeller/base/backend_cast.h"
#include "impeller/base/thread.h"
#include "impeller/renderer/backend/vulkan/compute_pipeline_vk.h"
#include "impeller/renderer/backend/vulkan/pipeline_cache_vk.h"
#include "impeller/renderer/backend/vulkan/pipeline_vk.h"
#include "impeller/renderer/backend/vulkan/vk.h"
#include "impeller/renderer/pipeline.h"
#include "impeller/renderer/pipeline_library.h"

namespace impeller {

class ContextVK;

class PipelineLibraryVK final
    : public PipelineLibrary,
      public BackendCast<PipelineLibraryVK, PipelineLibrary> {
 public:
  // |PipelineLibrary|
  ~PipelineLibraryVK() override;

  void DidAcquireSurfaceFrame();

  const std::shared_ptr<PipelineCacheVK>& GetPSOCache() const;

  const std::shared_ptr<fml::ConcurrentTaskRunner>& GetWorkerTaskRunner() const;

 private:
  friend ContextVK;

  std::weak_ptr<DeviceHolderVK> device_holder_;
  std::shared_ptr<PipelineCacheVK> pso_cache_;
  std::shared_ptr<fml::ConcurrentTaskRunner> worker_task_runner_;
  Mutex pipelines_mutex_;
  PipelineMap pipelines_ IPLR_GUARDED_BY(pipelines_mutex_);
  ComputePipelineMap compute_pipelines_ IPLR_GUARDED_BY(pipelines_mutex_);
  std::atomic_size_t frames_acquired_ = 0u;
  PipelineKey pipeline_key_ IPLR_GUARDED_BY(pipelines_mutex_) = 1;
  bool is_valid_ = false;
  bool cache_dirty_ = false;

  PipelineLibraryVK(
      const std::shared_ptr<DeviceHolderVK>& device_holder,
      std::shared_ptr<const Capabilities> caps,
      fml::UniqueFD cache_directory,
      std::shared_ptr<fml::ConcurrentTaskRunner> worker_task_runner);

  // |PipelineLibrary|
  bool IsValid() const override;

  // |PipelineLibrary|
  PipelineFuture<PipelineDescriptor> GetPipeline(PipelineDescriptor descriptor,
                                                 bool async) override;

  // |PipelineLibrary|
  PipelineFuture<ComputePipelineDescriptor> GetPipeline(
      ComputePipelineDescriptor descriptor,
      bool async) override;

  // |PipelineLibrary|
  bool HasPipeline(const PipelineDescriptor& descriptor) override;

  // |PipelineLibrary|
  void RemovePipelinesWithEntryPoint(
      std::shared_ptr<const ShaderFunction> function) override;

  std::unique_ptr<ComputePipelineVK> CreateComputePipeline(
      const ComputePipelineDescriptor& desc,
      PipelineKey pipeline_key);

  void PersistPipelineCacheToDisk();

  PipelineLibraryVK(const PipelineLibraryVK&) = delete;

  PipelineLibraryVK& operator=(const PipelineLibraryVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_PIPELINE_LIBRARY_VK_H_
