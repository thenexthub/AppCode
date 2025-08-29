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

#include "appcode/shell/platform/windows/task_runner.h"

#include <atomic>
#include <utility>

namespace appcode {

TaskRunner::TaskRunner(CurrentTimeProc get_current_time,
                       const TaskExpiredCallback& on_task_expired)
    : get_current_time_(get_current_time),
      on_task_expired_(std::move(on_task_expired)) {
  main_thread_id_ = GetCurrentThreadId();
  task_runner_window_ = TaskRunnerWindow::GetSharedInstance();
  task_runner_window_->AddDelegate(this);
}

TaskRunner::~TaskRunner() {
  task_runner_window_->RemoveDelegate(this);
}

std::chrono::nanoseconds TaskRunner::ProcessTasks() {
  const TaskTimePoint now = GetCurrentTimeForTask();

  std::vector<Task> expired_tasks;

  // Process expired tasks.
  {
    std::lock_guard<std::mutex> lock(task_queue_mutex_);
    while (!task_queue_.empty()) {
      const auto& top = task_queue_.top();
      // If this task (and all tasks after this) has not yet expired, there is
      // nothing more to do. Quit iterating.
      if (top.fire_time > now) {
        break;
      }

      // Make a record of the expired task. Do NOT service the task here
      // because we are still holding onto the task queue mutex. We don't want
      // other threads to block on posting tasks onto this thread till we are
      // done processing expired tasks.
      expired_tasks.push_back(task_queue_.top());

      // Remove the tasks from the delayed tasks queue.
      task_queue_.pop();
    }
  }

  // Fire expired tasks.
  {
    // Flushing tasks here without holing onto the task queue mutex.
    for (const auto& task : expired_tasks) {
      if (auto appcode_task = std::get_if<appcodeTask>(&task.variant)) {
        on_task_expired_(appcode_task);
      } else if (auto closure = std::get_if<TaskClosure>(&task.variant))
        (*closure)();
    }
  }

  // Calculate duration to sleep for on next iteration.
  {
    std::lock_guard<std::mutex> lock(task_queue_mutex_);
    const auto next_wake = task_queue_.empty() ? TaskTimePoint::max()
                                               : task_queue_.top().fire_time;

    return std::min(next_wake - now, std::chrono::nanoseconds::max());
  }
}

TaskRunner::TaskTimePoint TaskRunner::TimePointFromappcodeTime(
    uint64_t appcode_target_time_nanos) const {
  const auto now = GetCurrentTimeForTask();
  const auto appcode_duration = appcode_target_time_nanos - get_current_time_();
  return now + std::chrono::nanoseconds(appcode_duration);
}

void TaskRunner::PostappcodeTask(appcodeTask appcode_task,
                                 uint64_t appcode_target_time_nanos) {
  Task task;
  task.fire_time = TimePointFromappcodeTime(appcode_target_time_nanos);
  task.variant = appcode_task;
  EnqueueTask(std::move(task));
}

void TaskRunner::PostTask(TaskClosure closure) {
  Task task;
  task.fire_time = GetCurrentTimeForTask();
  task.variant = std::move(closure);
  EnqueueTask(std::move(task));
}

void TaskRunner::PollOnce(std::chrono::milliseconds timeout) {
  task_runner_window_->PollOnce(timeout);
}

void TaskRunner::EnqueueTask(Task task) {
  static std::atomic_uint64_t sGlobalTaskOrder(0);

  task.order = ++sGlobalTaskOrder;
  {
    std::lock_guard<std::mutex> lock(task_queue_mutex_);
    task_queue_.push(task);

    // Make sure the queue mutex is unlocked before waking up the loop. In case
    // the wake causes this thread to be descheduled for the primary thread to
    // process tasks, the acquisition of the lock on that thread while holding
    // the lock here momentarily till the end of the scope is a pessimization.
  }

  WakeUp();
}

bool TaskRunner::RunsTasksOnCurrentThread() const {
  return GetCurrentThreadId() == main_thread_id_;
}

void TaskRunner::WakeUp() {
  task_runner_window_->WakeUp();
}

}  // namespace appcode
