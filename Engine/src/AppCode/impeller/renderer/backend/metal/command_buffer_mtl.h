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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_COMMAND_BUFFER_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_COMMAND_BUFFER_MTL_H_

#include <Metal/Metal.h>

#include "impeller/core/allocator.h"
#include "impeller/renderer/command_buffer.h"

namespace impeller {

class CommandBufferMTL final : public CommandBuffer {
 public:
  // |CommandBuffer|
  ~CommandBufferMTL() override;

 private:
  friend class ContextMTL;

  id<MTLCommandBuffer> buffer_ = nil;
  id<MTLDevice> device_ = nil;

  CommandBufferMTL(const std::weak_ptr<const Context>& context,
                   id<MTLDevice> device,
                   id<MTLCommandQueue> queue);

  // |CommandBuffer|
  void SetLabel(std::string_view label) const override;

  // |CommandBuffer|
  bool IsValid() const override;

  // |CommandBuffer|
  bool OnSubmitCommands(CompletionCallback callback) override;

  // |CommandBuffer|
  void OnWaitUntilCompleted() override;

  // |CommandBuffer|
  void OnWaitUntilScheduled() override;

  // |CommandBuffer|
  std::shared_ptr<RenderPass> OnCreateRenderPass(RenderTarget target) override;

  // |CommandBuffer|
  std::shared_ptr<BlitPass> OnCreateBlitPass() override;

  // |CommandBuffer|
  std::shared_ptr<ComputePass> OnCreateComputePass() override;

  CommandBufferMTL(const CommandBufferMTL&) = delete;

  CommandBufferMTL& operator=(const CommandBufferMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_COMMAND_BUFFER_MTL_H_
