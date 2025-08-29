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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_PIPELINE_LIBRARY_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_PIPELINE_LIBRARY_MTL_H_

#include <Metal/Metal.h>

#include "impeller/renderer/pipeline_library.h"

namespace impeller {

class ContextMTL;

class PipelineLibraryMTL final : public PipelineLibrary {
 public:
  PipelineLibraryMTL();

  // |PipelineLibrary|
  ~PipelineLibraryMTL() override;

 private:
  friend ContextMTL;

  id<MTLDevice> device_ = nullptr;
  PipelineMap pipelines_;
  ComputePipelineMap compute_pipelines_;

  explicit PipelineLibraryMTL(id<MTLDevice> device);

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

  PipelineLibraryMTL(const PipelineLibraryMTL&) = delete;

  PipelineLibraryMTL& operator=(const PipelineLibraryMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_PIPELINE_LIBRARY_MTL_H_
