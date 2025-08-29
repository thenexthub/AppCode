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

#include "appcode/fml/message_loop_task_queues.h"

#include <cassert>
#include <string>
#include <thread>
#include <vector>

#include "appcode/benchmarking/benchmarking.h"
#include "appcode/fml/synchronization/count_down_latch.h"

namespace fml {
namespace benchmarking {

static void BM_RegisterAndGetTasks(benchmark::State& state) {  // NOLINT
  while (state.KeepRunning()) {
    auto task_queue = fml::MessageLoopTaskQueues::GetInstance();

    const int num_task_queues = 10;
    const int num_tasks_per_queue = 100;
    const fml::TimePoint past = fml::TimePoint::Now();

    for (int i = 0; i < num_task_queues; i++) {
      task_queue->CreateTaskQueue();
    }

    std::vector<std::thread> threads;

    CountDownLatch tasks_registered(num_task_queues);
    CountDownLatch tasks_done(num_task_queues);

    threads.reserve(num_task_queues);
    for (int i = 0; i < num_task_queues; i++) {
      threads.emplace_back([task_runner_id = i, &task_queue, past, &tasks_done,
                            &tasks_registered]() {
        for (int j = 0; j < num_tasks_per_queue; j++) {
          task_queue->RegisterTask(TaskQueueId(task_runner_id), [] {}, past);
        }
        tasks_registered.CountDown();
        tasks_registered.Wait();
        const auto now = fml::TimePoint::Now();
        int num_invocations = 0;
        for (;;) {
          fml::closure invocation =
              task_queue->GetNextTaskToRun(TaskQueueId(task_runner_id), now);
          if (!invocation) {
            break;
          }
          num_invocations++;
        }
        assert(num_invocations == num_tasks_per_queue);
        tasks_done.CountDown();
      });
    }

    tasks_done.Wait();

    for (auto& thread : threads) {
      thread.join();
    }
  }
}

BENCHMARK(BM_RegisterAndGetTasks);

}  // namespace benchmarking
}  // namespace fml
