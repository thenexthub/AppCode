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

#ifndef APPCODE_FML_MEMORY_TASK_RUNNER_CHECKER_H_
#define APPCODE_FML_MEMORY_TASK_RUNNER_CHECKER_H_

#include "appcode/fml/message_loop.h"
#include "appcode/fml/task_runner.h"

namespace fml {

class TaskRunnerChecker final {
 public:
  TaskRunnerChecker();

  ~TaskRunnerChecker();

  bool RunsOnCreationTaskRunner() const;

  static bool RunsOnTheSameThread(TaskQueueId queue_a, TaskQueueId queue_b);

 private:
  TaskQueueId initialized_queue_id_;
  std::set<TaskQueueId> subsumed_queue_ids_;

  TaskQueueId InitTaskQueueId();
};

#if !defined(NDEBUG)
#define FML_DECLARE_TASK_RUNNER_CHECKER(c) fml::TaskRunnerChecker c
#define FML_DCHECK_TASK_RUNNER_IS_CURRENT(c) \
  FML_DCHECK((c).RunsOnCreationTaskRunner())
#else
#define FML_DECLARE_TASK_RUNNER_CHECKER(c)
#define FML_DCHECK_TASK_RUNNER_IS_CURRENT(c) ((void)0)
#endif

}  // namespace fml

#endif  // APPCODE_FML_MEMORY_TASK_RUNNER_CHECKER_H_
