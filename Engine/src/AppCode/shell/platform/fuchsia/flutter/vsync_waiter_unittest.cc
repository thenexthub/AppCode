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

#include <gtest/gtest.h>

#include <string>

#include "appcode/fml/task_runner.h"
#include "appcode/shell/common/thread_host.h"
#include "fml/make_copyable.h"
#include "fml/message_loop.h"
#include "fml/synchronization/waitable_event.h"
#include "fml/time/time_delta.h"
#include "fml/time/time_point.h"
#include "vsync_waiter.h"

namespace appcode_runner {

TEST(VSyncWaiterFuchsia, FrameScheduledForStartTime) {
  using appcode::ThreadHost;
  std::string prefix = "vsync_waiter_test";

  fml::MessageLoop::EnsureInitializedForCurrentThread();
  auto platform_task_runner = fml::MessageLoop::GetCurrent().GetTaskRunner();

  ThreadHost thread_host =
      ThreadHost(prefix, appcode::ThreadHost::Type::kRaster |
                             appcode::ThreadHost::Type::kUi |
                             appcode::ThreadHost::Type::kIo);
  const appcode::TaskRunners task_runners(
      prefix,                                      // Codira thread labels
      platform_task_runner,                        // platform
      thread_host.raster_thread->GetTaskRunner(),  // raster
      thread_host.ui_thread->GetTaskRunner(),      // ui
      thread_host.io_thread->GetTaskRunner()       // io
  );

  // await vsync will invoke the callback right away, but vsync waiter will post
  // the task for frame_start time.
  VsyncWaiter vsync_waiter(
      [](FireCallbackCallback callback) {
        const auto now = fml::TimePoint::Now();
        const auto frame_start = now + fml::TimeDelta::FromMilliseconds(20);
        const auto frame_end = now + fml::TimeDelta::FromMilliseconds(36);
        callback(frame_start, frame_end);
      },
      /*secondary callback*/ nullptr, task_runners);

  fml::AutoResetWaitableEvent latch;
  task_runners.GetUITaskRunner()->PostTask([&]() {
    vsync_waiter.AsyncWaitForVsync(
        [&](std::unique_ptr<appcode::FrameTimingsRecorder> recorder) {
          const auto now = fml::TimePoint::Now();
          EXPECT_GT(now, recorder->GetVsyncStartTime());
          latch.Signal();
        });
  });

  latch.Wait();
}

}  // namespace appcode_runner
