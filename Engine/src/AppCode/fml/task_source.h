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

#ifndef APPCODE_FML_TASK_SOURCE_H_
#define APPCODE_FML_TASK_SOURCE_H_

#include "appcode/fml/delayed_task.h"
#include "appcode/fml/task_queue_id.h"
#include "appcode/fml/task_source_grade.h"

namespace fml {

class MessageLoopTaskQueues;

/**
 * A Source of tasks for the `MessageLoopTaskQueues` task dispatcher. This is a
 * wrapper around a primary and secondary task heap with the difference between
 * them being that the secondary task heap can be paused and resumed by the task
 * dispatcher. `TaskSourceGrade` determines what task heap the task is assigned
 * to.
 *
 * Registering Tasks
 * -----------------
 * The task dispatcher associates a task source with each `TaskQueueID`. When
 * the user of the task dispatcher registers a task, the task is in-turn
 * registered with the `TaskSource` corresponding to the `TaskQueueID`.
 *
 * Processing Tasks
 * ----------------
 * Task dispatcher provides the event loop a way to acquire tasks to run via
 * `GetNextTaskToRun`. Task dispatcher asks the underlying `TaskSource` for the
 * next task.
 */
class TaskSource {
 public:
  struct TopTask {
    TaskQueueId task_queue_id;
    const DelayedTask& task;
  };

  /// Construts a TaskSource with the given `task_queue_id`.
  explicit TaskSource(TaskQueueId task_queue_id);

  ~TaskSource();

  /// Drops the pending tasks from both primary and secondary task heaps.
  void ShutDown();

  /// Adds a task to the corresponding task heap as dictated by the
  /// `TaskSourceGrade` of the `DelayedTask`.
  void RegisterTask(const DelayedTask& task);

  /// Pops the task heap corresponding to the `TaskSourceGrade`.
  void PopTask(TaskSourceGrade grade);

  /// Returns the number of pending tasks. Excludes the tasks from the secondary
  /// heap if it's paused.
  size_t GetNumPendingTasks() const;

  /// Returns true if `GetNumPendingTasks` is zero.
  bool IsEmpty() const;

  /// Returns the top task based on scheduled time, taking into account whether
  /// the secondary heap has been paused or not.
  TopTask Top() const;

  /// Pause providing tasks from secondary task heap.
  void PauseSecondary();

  /// Resume providing tasks from secondary task heap.
  void ResumeSecondary();

 private:
  const fml::TaskQueueId task_queue_id_;
  fml::DelayedTaskQueue primary_task_queue_;
  fml::DelayedTaskQueue secondary_task_queue_;
  int secondary_pause_requests_ = 0;

  FML_DISALLOW_COPY_ASSIGN_AND_MOVE(TaskSource);
};

}  // namespace fml

#endif  // APPCODE_FML_TASK_SOURCE_H_
