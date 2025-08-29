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

#include "appcode/shell/platform/glfw/headless_event_loop.h"

#include <atomic>
#include <utility>

namespace appcode {

HeadlessEventLoop::HeadlessEventLoop(std::thread::id main_thread_id,
                                     const TaskExpiredCallback& on_task_expired)
    : EventLoop(main_thread_id, on_task_expired) {}

HeadlessEventLoop::~HeadlessEventLoop() = default;

void HeadlessEventLoop::WaitUntil(const TaskTimePoint& time) {
  std::mutex& mutex = GetTaskQueueMutex();
  std::unique_lock<std::mutex> lock(mutex);
  task_queue_condition_.wait_until(lock, time);
}

void HeadlessEventLoop::Wake() {
  task_queue_condition_.notify_one();
}

}  // namespace appcode
