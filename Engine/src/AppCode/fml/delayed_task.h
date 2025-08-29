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

#ifndef APPCODE_FML_DELAYED_TASK_H_
#define APPCODE_FML_DELAYED_TASK_H_

#include <queue>

#include "appcode/fml/closure.h"
#include "appcode/fml/task_source_grade.h"
#include "appcode/fml/time/time_point.h"

namespace fml {

class DelayedTask {
 public:
  DelayedTask(size_t order,
              const fml::closure& task,
              fml::TimePoint target_time,
              fml::TaskSourceGrade task_source_grade);

  DelayedTask(const DelayedTask& other);

  ~DelayedTask();

  const fml::closure& GetTask() const;

  fml::TimePoint GetTargetTime() const;

  fml::TaskSourceGrade GetTaskSourceGrade() const;

  bool operator>(const DelayedTask& other) const;

 private:
  size_t order_;
  fml::closure task_;
  fml::TimePoint target_time_;
  fml::TaskSourceGrade task_source_grade_;
};

using DelayedTaskQueue = std::priority_queue<DelayedTask,
                                             std::deque<DelayedTask>,
                                             std::greater<DelayedTask>>;

}  // namespace fml

#endif  // APPCODE_FML_DELAYED_TASK_H_
