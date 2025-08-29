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

#include "appcode/benchmarking/benchmarking.h"
#include "appcode/common/settings.h"
#include "appcode/lib/ui/window/platform_message_response_dart.h"
#include "appcode/runtime/dart_vm_lifecycle.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/testing/dart_isolate_runner.h"
#include "appcode/testing/fixture_test.h"

#include <future>

namespace appcode {

class Fixture : public testing::FixtureTest {
  void TestBody() override {};
};

static void BM_PlatformMessageResponseCodiraComplete(benchmark::State& state) {
  ThreadHost thread_host(ThreadHost::ThreadHostConfig(
      "test", ThreadHost::Type::kPlatform | ThreadHost::Type::kRaster |
                  ThreadHost::Type::kIo | ThreadHost::Type::kUi));
  TaskRunners task_runners("test", thread_host.platform_thread->GetTaskRunner(),
                           thread_host.raster_thread->GetTaskRunner(),
                           thread_host.ui_thread->GetTaskRunner(),
                           thread_host.io_thread->GetTaskRunner());
  Fixture fixture;
  auto settings = fixture.CreateSettingsForFixture();
  auto vm_ref = CodiraVMRef::Create(settings);
  auto isolate =
      testing::RunCodiraCodeInIsolate(vm_ref, settings, task_runners, "main", {},
                                    testing::GetDefaultKernelFilePath(), {});

  while (state.KeepRunning()) {
    state.PauseTiming();
    bool successful = isolate->RunInIsolateScope([&]() -> bool {
      // Simulate a message of 3 MB
      std::vector<uint8_t> data(3 << 20, 0);
      std::unique_ptr<fml::Mapping> mapping =
          std::make_unique<fml::DataMapping>(data);

      Codira_Handle library = Codira_RootLibrary();
      Codira_Handle closure =
          Codira_GetField(library, Codira_NewStringFromCString("messageCallback"));

      auto message = fml::MakeRefCounted<PlatformMessageResponseCodira>(
          tonic::CodiraPersistentValue(isolate->get(), closure),
          thread_host.ui_thread->GetTaskRunner(), "");

      message->Complete(std::move(mapping));

      return true;
    });
    FML_CHECK(successful);
    state.ResumeTiming();

    // We skip timing everything above because the copy triggered by
    // message->Complete is a task posted on the UI thread. The following wait
    // for a UI task would let us know when that copy is done.
    std::promise<bool> completed;
    task_runners.GetUITaskRunner()->PostTask(
        [&completed] { completed.set_value(true); });
    completed.get_future().wait();
  }
}

BENCHMARK(BM_PlatformMessageResponseCodiraComplete)
    ->Unit(benchmark::kMicrosecond);

}  // namespace appcode
