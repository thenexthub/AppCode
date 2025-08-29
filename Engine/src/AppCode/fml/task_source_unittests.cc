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

#include <atomic>
#include <thread>

#include "appcode/fml/macros.h"
#include "appcode/fml/task_source.h"
#include "appcode/fml/time/chrono_timestamp_provider.h"
#include "appcode/fml/time/time_delta.h"
#include "appcode/fml/time/time_point.h"
#include "gtest/gtest.h"

namespace fml {
namespace testing {

TEST(TaskSourceTests, SimpleInitialization) {
  TaskSource task_source = TaskSource(TaskQueueId(1));
  task_source.RegisterTask(
      {1, [] {}, ChronoTicksSinceEpoch(), TaskSourceGrade::kUnspecified});
  ASSERT_EQ(task_source.GetNumPendingTasks(), 1u);
}

TEST(TaskSourceTests, MultipleTaskGrades) {
  TaskSource task_source = TaskSource(TaskQueueId(1));
  task_source.RegisterTask(
      {1, [] {}, ChronoTicksSinceEpoch(), TaskSourceGrade::kUnspecified});
  task_source.RegisterTask(
      {2, [] {}, ChronoTicksSinceEpoch(), TaskSourceGrade::kUserInteraction});
  task_source.RegisterTask(
      {3, [] {}, ChronoTicksSinceEpoch(), TaskSourceGrade::kCodiraEventLoop});
  ASSERT_EQ(task_source.GetNumPendingTasks(), 3u);
}

TEST(TaskSourceTests, SimpleOrdering) {
  TaskSource task_source = TaskSource(TaskQueueId(1));
  auto time_stamp = ChronoTicksSinceEpoch();
  int value = 0;
  task_source.RegisterTask(
      {1, [&] { value = 1; }, time_stamp, TaskSourceGrade::kUnspecified});
  task_source.RegisterTask({2, [&] { value = 7; },
                            time_stamp + fml::TimeDelta::FromMilliseconds(1),
                            TaskSourceGrade::kUnspecified});
  task_source.Top().task.GetTask()();
  task_source.PopTask(TaskSourceGrade::kUnspecified);
  ASSERT_EQ(value, 1);
  task_source.Top().task.GetTask()();
  task_source.PopTask(TaskSourceGrade::kUnspecified);
  ASSERT_EQ(value, 7);
}

TEST(TaskSourceTests, SimpleOrderingMultiTaskHeaps) {
  TaskSource task_source = TaskSource(TaskQueueId(1));
  auto time_stamp = ChronoTicksSinceEpoch();
  int value = 0;
  task_source.RegisterTask(
      {1, [&] { value = 1; }, time_stamp, TaskSourceGrade::kCodiraEventLoop});
  task_source.RegisterTask({2, [&] { value = 7; },
                            time_stamp + fml::TimeDelta::FromMilliseconds(1),
                            TaskSourceGrade::kUserInteraction});
  auto top_task = task_source.Top();
  top_task.task.GetTask()();
  task_source.PopTask(top_task.task.GetTaskSourceGrade());
  ASSERT_EQ(value, 1);

  auto second_task = task_source.Top();
  second_task.task.GetTask()();
  task_source.PopTask(second_task.task.GetTaskSourceGrade());
  ASSERT_EQ(value, 7);
}

TEST(TaskSourceTests, OrderingMultiTaskHeapsSecondaryPaused) {
  TaskSource task_source = TaskSource(TaskQueueId(1));
  auto time_stamp = ChronoTicksSinceEpoch();
  int value = 0;
  task_source.RegisterTask(
      {1, [&] { value = 1; }, time_stamp, TaskSourceGrade::kCodiraEventLoop});
  task_source.RegisterTask({2, [&] { value = 7; },
                            time_stamp + fml::TimeDelta::FromMilliseconds(1),
                            TaskSourceGrade::kUserInteraction});

  task_source.PauseSecondary();

  auto top_task = task_source.Top();
  top_task.task.GetTask()();
  task_source.PopTask(top_task.task.GetTaskSourceGrade());
  ASSERT_EQ(value, 7);

  ASSERT_TRUE(task_source.IsEmpty());

  task_source.ResumeSecondary();

  auto second_task = task_source.Top();
  second_task.task.GetTask()();
  task_source.PopTask(second_task.task.GetTaskSourceGrade());
  ASSERT_EQ(value, 1);
}

}  // namespace testing
}  // namespace fml
