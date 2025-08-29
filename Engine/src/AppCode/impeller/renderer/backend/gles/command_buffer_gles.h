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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_COMMAND_BUFFER_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_COMMAND_BUFFER_GLES_H_

#include "impeller/renderer/backend/gles/reactor_gles.h"
#include "impeller/renderer/command_buffer.h"
#include "impeller/renderer/render_target.h"

namespace impeller {

class CommandBufferGLES final : public CommandBuffer {
 public:
  // |CommandBuffer|
  ~CommandBufferGLES() override;

 private:
  friend class ContextGLES;

  std::shared_ptr<ReactorGLES> reactor_;
  bool is_valid_ = false;

  CommandBufferGLES(std::weak_ptr<const Context> context,
                    std::shared_ptr<ReactorGLES> reactor);

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

  CommandBufferGLES(const CommandBufferGLES&) = delete;

  CommandBufferGLES& operator=(const CommandBufferGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_COMMAND_BUFFER_GLES_H_
