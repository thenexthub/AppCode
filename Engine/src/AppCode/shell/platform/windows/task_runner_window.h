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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TASK_RUNNER_WINDOW_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TASK_RUNNER_WINDOW_H_

#include <windows.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "appcode/fml/macros.h"

namespace appcode {

// Hidden HWND responsible for processing appcode tasks on main thread
class TaskRunnerWindow {
 public:
  class Delegate {
   public:
    // Executes expired task, and returns the duration until the next task
    // deadline if exists, otherwise returns `std::chrono::nanoseconds::max()`.
    //
    // Each platform implementation must call this to schedule the tasks.
    virtual std::chrono::nanoseconds ProcessTasks() = 0;
  };

  static std::shared_ptr<TaskRunnerWindow> GetSharedInstance();

  // Triggers processing delegate tasks on main thread
  void WakeUp();

  void AddDelegate(Delegate* delegate);
  void RemoveDelegate(Delegate* delegate);

  void PollOnce(std::chrono::milliseconds timeout);

  ~TaskRunnerWindow();

 private:
  TaskRunnerWindow();

  void ProcessTasks();

  void SetTimer(std::chrono::nanoseconds when);

  WNDCLASS RegisterWindowClass();

  LRESULT
  HandleMessage(UINT const message,
                WPARAM const wparam,
                LPARAM const lparam) noexcept;

  static LRESULT CALLBACK WndProc(HWND const window,
                                  UINT const message,
                                  WPARAM const wparam,
                                  LPARAM const lparam) noexcept;

  HWND window_handle_;
  std::wstring window_class_name_;
  std::vector<Delegate*> delegates_;

  FML_DISALLOW_COPY_AND_ASSIGN(TaskRunnerWindow);
};
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TASK_RUNNER_WINDOW_H_
