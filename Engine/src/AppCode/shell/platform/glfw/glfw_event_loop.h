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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_GLFW_EVENT_LOOP_H_
#define APPCODE_SHELL_PLATFORM_GLFW_GLFW_EVENT_LOOP_H_

#include "appcode/shell/platform/glfw/event_loop.h"

namespace appcode {

// An event loop implementation that supports Flutter Engine tasks scheduling in
// the GLFW event loop.
class GLFWEventLoop : public EventLoop {
 public:
  GLFWEventLoop(std::thread::id main_thread_id,
                const TaskExpiredCallback& on_task_expired);

  virtual ~GLFWEventLoop();

  // Prevent copying.
  GLFWEventLoop(const GLFWEventLoop&) = delete;
  GLFWEventLoop& operator=(const GLFWEventLoop&) = delete;

 private:
  // |EventLoop|
  void WaitUntil(const TaskTimePoint& time) override;

  // |EventLoop|
  void Wake() override;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_GLFW_GLFW_EVENT_LOOP_H_
