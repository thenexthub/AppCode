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

#include "appcode/shell/platform/glfw/glfw_event_loop.h"

#include <GLFW/glfw3.h>

#include <atomic>
#include <utility>

namespace appcode {

GLFWEventLoop::GLFWEventLoop(std::thread::id main_thread_id,
                             const TaskExpiredCallback& on_task_expired)
    : EventLoop(main_thread_id, on_task_expired) {}

GLFWEventLoop::~GLFWEventLoop() = default;

void GLFWEventLoop::WaitUntil(const TaskTimePoint& time) {
  const auto now = TaskTimePoint::clock::now();

  // Make sure the seconds are not integral.
  using Seconds = std::chrono::duration<double, std::ratio<1>>;
  const auto duration_to_wait = std::chrono::duration_cast<Seconds>(time - now);

  if (duration_to_wait.count() > 0.0) {
    ::glfwWaitEventsTimeout(duration_to_wait.count());
  } else {
    // Avoid engine task priority inversion by making sure GLFW events are
    // always processed even when there is no need to wait for pending engine
    // tasks.
    ::glfwPollEvents();
  }
}

void GLFWEventLoop::Wake() {
  ::glfwPostEmptyEvent();
}

}  // namespace appcode
