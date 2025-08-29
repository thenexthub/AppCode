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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_COMPUTE_PIPELINE_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_COMPUTE_PIPELINE_MTL_H_

#include <Metal/Metal.h>

#include "impeller/base/backend_cast.h"
#include "impeller/renderer/pipeline.h"

namespace impeller {

class ComputePipelineMTL final
    : public Pipeline<ComputePipelineDescriptor>,
      public BackendCast<ComputePipelineMTL,
                         Pipeline<ComputePipelineDescriptor>> {
 public:
  // |Pipeline|
  ~ComputePipelineMTL() override;

  id<MTLComputePipelineState> GetMTLComputePipelineState() const;

 private:
  friend class PipelineLibraryMTL;

  id<MTLComputePipelineState> pipeline_state_;
  bool is_valid_ = false;

  ComputePipelineMTL(std::weak_ptr<PipelineLibrary> library,
                     const ComputePipelineDescriptor& desc,
                     id<MTLComputePipelineState> state);

  // |Pipeline|
  bool IsValid() const override;

  ComputePipelineMTL(const ComputePipelineMTL&) = delete;

  ComputePipelineMTL& operator=(const ComputePipelineMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_COMPUTE_PIPELINE_MTL_H_
