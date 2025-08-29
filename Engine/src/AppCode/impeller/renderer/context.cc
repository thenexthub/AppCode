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

#include "impeller/renderer/context.h"

#include <mutex>
#include <utility>

namespace impeller {

ImpellerContextFuture::ImpellerContextFuture(
    std::future<std::shared_ptr<impeller::Context>> context)
    : future_(std::move(context)) {}

std::shared_ptr<impeller::Context> ImpellerContextFuture::GetContext() {
  std::scoped_lock<std::mutex> lock(mutex_);
  if (!did_wait_ && future_.valid()) {
    context_ = future_.get();
    did_wait_ = true;
  }
  return context_;
}

Context::~Context() = default;

Context::Context(const Flags& flags) : flags_(flags) {}

bool Context::UpdateOffscreenLayerPixelFormat(PixelFormat format) {
  return false;
}

bool Context::EnqueueCommandBuffer(
    std::shared_ptr<CommandBuffer> command_buffer) {
  return GetCommandQueue()->Submit({std::move(command_buffer)}).ok();
}

bool Context::FlushCommandBuffers() {
  return true;
}

std::shared_ptr<const IdleWaiter> Context::GetIdleWaiter() const {
  return nullptr;
}

void Context::ResetThreadLocalState() const {
  // Nothing to do.
}

bool Context::AddTrackingFence(const std::shared_ptr<Texture>& texture) const {
  return false;
}

bool Context::SubmitOnscreen(std::shared_ptr<CommandBuffer> cmd_buffer) {
  return EnqueueCommandBuffer(std::move(cmd_buffer));
}

}  // namespace impeller
