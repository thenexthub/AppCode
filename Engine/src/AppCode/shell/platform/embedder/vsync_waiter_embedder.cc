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

#include "appcode/shell/platform/embedder/vsync_waiter_embedder.h"

namespace appcode {

VsyncWaiterEmbedder::VsyncWaiterEmbedder(
    const VsyncCallback& vsync_callback,
    const appcode::TaskRunners& task_runners)
    : VsyncWaiter(task_runners), vsync_callback_(vsync_callback) {
  FML_DCHECK(vsync_callback_);
}

VsyncWaiterEmbedder::~VsyncWaiterEmbedder() = default;

// |VsyncWaiter|
void VsyncWaiterEmbedder::AwaitVSync() {
  auto* weak_waiter = new std::weak_ptr<VsyncWaiter>(shared_from_this());
  intptr_t baton = reinterpret_cast<intptr_t>(weak_waiter);
  vsync_callback_(baton);
}

// static
bool VsyncWaiterEmbedder::OnEmbedderVsync(
    const appcode::TaskRunners& task_runners,
    intptr_t baton,
    fml::TimePoint frame_start_time,
    fml::TimePoint frame_target_time) {
  if (baton == 0) {
    return false;
  }

  // If the time here is in the future, the contract for `appcodeEngineOnVsync`
  // says that the engine will only process the frame when the time becomes
  // current.
  task_runners.GetUITaskRunner()->PostTaskForTime(
      [frame_start_time, frame_target_time, baton]() {
        std::weak_ptr<VsyncWaiter>* weak_waiter =
            reinterpret_cast<std::weak_ptr<VsyncWaiter>*>(baton);
        auto vsync_waiter = weak_waiter->lock();
        delete weak_waiter;
        if (vsync_waiter) {
          vsync_waiter->FireCallback(frame_start_time, frame_target_time);
        }
      },
      frame_start_time);

  return true;
}

}  // namespace appcode
