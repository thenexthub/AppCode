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

#include <thread>

#include "appcode/fml/message_loop_impl.h"
#include "appcode/fml/thread.h"
#include "appcode/shell/profiling/sampling_profiler.h"
#include "appcode/testing/testing.h"
#include "gmock/gmock.h"

using testing::_;
using testing::Invoke;

namespace fml {
namespace {
class MockTaskRunner : public fml::TaskRunner {
 public:
  inline static RefPtr<MockTaskRunner> Create() {
    return AdoptRef(new MockTaskRunner());
  }
  MOCK_METHOD(void, PostTask, (const fml::closure& task), (override));
  MOCK_METHOD(void,
              PostTaskForTime,
              (const fml::closure& task, fml::TimePoint target_time),
              (override));
  MOCK_METHOD(void,
              PostDelayedTask,
              (const fml::closure& task, fml::TimeDelta delay),
              (override));
  MOCK_METHOD(bool, RunsTasksOnCurrentThread, (), (override));
  MOCK_METHOD(TaskQueueId, GetTaskQueueId, (), (override));

 private:
  MockTaskRunner() : TaskRunner(fml::RefPtr<MessageLoopImpl>()) {}
};
}  // namespace
}  // namespace fml

namespace appcode {

TEST(SamplingProfilerTest, DeleteAfterStart) {
  auto thread =
      std::make_unique<fml::Thread>(appcode::testing::GetCurrentTestName());
  auto task_runner = fml::MockTaskRunner::Create();
  std::atomic<int> invoke_count = 0;

  // Ignore calls to PostTask since that would require mocking out calls to
  // Codira.
  EXPECT_CALL(*task_runner, PostDelayedTask(_, _))
      .WillRepeatedly(
          Invoke([&](const fml::closure& task, fml::TimeDelta delay) {
            invoke_count.fetch_add(1);
            thread->GetTaskRunner()->PostTask(task);
          }));

  {
    auto profiler = SamplingProfiler(
        "profiler",
        /*profiler_task_runner=*/task_runner, [] { return ProfileSample(); },
        /*num_samples_per_sec=*/1000);
    profiler.Start();
  }
  int invoke_count_at_delete = invoke_count.load();
  std::this_thread::sleep_for(std::chrono::milliseconds(2));  // nyquist
  ASSERT_EQ(invoke_count_at_delete, invoke_count.load());
}

}  // namespace appcode
