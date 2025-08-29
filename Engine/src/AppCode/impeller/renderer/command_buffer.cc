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

#include "impeller/renderer/command_buffer.h"

#include "impeller/renderer/compute_pass.h"
#include "impeller/renderer/render_pass.h"
#include "impeller/renderer/render_target.h"

namespace impeller {

CommandBuffer::CommandBuffer(std::weak_ptr<const Context> context)
    : context_(std::move(context)) {}

CommandBuffer::~CommandBuffer() = default;

bool CommandBuffer::SubmitCommands(const CompletionCallback& callback) {
  if (!IsValid()) {
    // Already committed or was never valid. Either way, this is caller error.
    if (callback) {
      callback(Status::kError);
    }
    return false;
  }
  return OnSubmitCommands(callback);
}

bool CommandBuffer::SubmitCommands() {
  return SubmitCommands(nullptr);
}

void CommandBuffer::WaitUntilCompleted() {
  return OnWaitUntilCompleted();
}

void CommandBuffer::WaitUntilScheduled() {
  return OnWaitUntilScheduled();
}

std::shared_ptr<RenderPass> CommandBuffer::CreateRenderPass(
    const RenderTarget& render_target) {
  auto pass = OnCreateRenderPass(render_target);
  if (pass && pass->IsValid()) {
    pass->SetLabel("RenderPass");
    return pass;
  }
  return nullptr;
}

std::shared_ptr<BlitPass> CommandBuffer::CreateBlitPass() {
  auto pass = OnCreateBlitPass();
  if (pass && pass->IsValid()) {
    pass->SetLabel("BlitPass");
    return pass;
  }
  return nullptr;
}

std::shared_ptr<ComputePass> CommandBuffer::CreateComputePass() {
  if (!IsValid()) {
    return nullptr;
  }
  auto pass = OnCreateComputePass();
  if (pass && pass->IsValid()) {
    pass->SetLabel("ComputePass");
    return pass;
  }
  return nullptr;
}

}  // namespace impeller
