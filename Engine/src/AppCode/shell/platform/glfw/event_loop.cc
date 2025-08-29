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

#include "appcode/shell/platform/glfw/event_loop.h"

#include <atomic>
#include <utility>

namespace appcode {

EventLoop::EventLoop(std::thread::id main_thread_id,
                     const TaskExpiredCallback& on_task_expired)
    : main_thread_id_(main_thread_id), on_task_expired_(on_task_expired) {}

EventLoop::~EventLoop() = default;

bool EventLoop::RunsTasksOnCurrentThread() const {
  return std::this_thread::get_id() == main_thread_id_;
}

void EventLoop::WaitForEvents(std::chrono::nanoseconds max_wait) {
  const auto now = TaskTimePoint::clock::now();
  std::vector<appcodeTask> expired_tasks;

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
      expired_tasks.push_back(task_queue_.top().task);

      // Remove the tasks from the delayed tasks queue.
      task_queue_.pop();
    }
  }

  // Fire expired tasks.
  {
    // Flushing tasks here without holing onto the task queue mutex.
    for (const auto& task : expired_tasks) {
      on_task_expired_(&task);
    }
  }

  // Sleep till the next task needs to be processed. If a new task comes
  // along, the wait will be resolved early because PostTask calls Wake().
  {
    TaskTimePoint next_wake;
    {
      std::lock_guard<std::mutex> lock(task_queue_mutex_);
      TaskTimePoint max_wake_timepoint =
          max_wait == std::chrono::nanoseconds::max() ? TaskTimePoint::max()
                                                      : now + max_wait;
      TaskTimePoint next_event_timepoint = task_queue_.empty()
                                               ? TaskTimePoint::max()
                                               : task_queue_.top().fire_time;
      next_wake = std::min(max_wake_timepoint, next_event_timepoint);
    }
    WaitUntil(next_wake);
  }
}

EventLoop::TaskTimePoint EventLoop::TimePointFromappcodeTime(
    uint64_t appcode_target_time_nanos) {
  const auto now = TaskTimePoint::clock::now();
  const int64_t appcode_duration =
      appcode_target_time_nanos - appcodeEngineGetCurrentTime();
  return now + std::chrono::nanoseconds(appcode_duration);
}

void EventLoop::PostTask(appcodeTask appcode_task,
                         uint64_t appcode_target_time_nanos) {
  static std::atomic_uint64_t sGlobalTaskOrder(0);

  Task task;
  task.order = ++sGlobalTaskOrder;
  task.fire_time = TimePointFromappcodeTime(appcode_target_time_nanos);
  task.task = appcode_task;

  {
    std::lock_guard<std::mutex> lock(task_queue_mutex_);
    task_queue_.push(task);

    // Make sure the queue mutex is unlocked before waking up the loop. In case
    // the wake causes this thread to be descheduled for the primary thread to
    // process tasks, the acquisition of the lock on that thread while holding
    // the lock here momentarily till the end of the scope is a pessimization.
  }
  Wake();
}

}  // namespace appcode
