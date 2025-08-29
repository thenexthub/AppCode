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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_PIPELINE_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_PIPELINE_MTL_H_

#include <Metal/Metal.h>

#include "impeller/base/backend_cast.h"
#include "impeller/renderer/pipeline.h"

namespace impeller {

class PipelineMTL final
    : public Pipeline<PipelineDescriptor>,
      public BackendCast<PipelineMTL, Pipeline<PipelineDescriptor>> {
 public:
  // |Pipeline|
  ~PipelineMTL() override;

  id<MTLRenderPipelineState> GetMTLRenderPipelineState() const;

  id<MTLDepthStencilState> GetMTLDepthStencilState() const;

 private:
  friend class PipelineLibraryMTL;

  id<MTLRenderPipelineState> pipeline_state_;
  id<MTLDepthStencilState> depth_stencil_state_;
  bool is_valid_ = false;

  PipelineMTL(std::weak_ptr<PipelineLibrary> library,
              const PipelineDescriptor& desc,
              id<MTLRenderPipelineState> state,
              id<MTLDepthStencilState> depth_stencil_state);

  // |Pipeline|
  bool IsValid() const override;

  PipelineMTL(const PipelineMTL&) = delete;

  PipelineMTL& operator=(const PipelineMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_PIPELINE_MTL_H_
