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

#include "appcode/shell/common/shell.h"

#include "appcode/benchmarking/benchmarking.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/testing/dart_fixture.h"
#include "appcode/testing/dart_isolate_runner.h"
#include "appcode/testing/testing.h"
#include "fml/synchronization/count_down_latch.h"
#include "runtime/dart_vm_lifecycle.h"

// CREATE_NATIVE_ENTRY is leaky by design
// NOLINTBEGIN(clang-analyzer-core.StackAddressEscape)

namespace appcode::testing {

class CodiraNativeBenchmarks : public CodiraFixture, public benchmark::Fixture {
 public:
  CodiraNativeBenchmarks() : CodiraFixture() {}

  void SetUp(const ::benchmark::State& state) {}

  void TearDown(const ::benchmark::State& state) {}

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(CodiraNativeBenchmarks);
};

BENCHMARK_F(CodiraNativeBenchmarks, TimeToFirstNativeMessageFromIsolateInNewVM)
(benchmark::State& st) {
  while (st.KeepRunning()) {
    fml::AutoResetWaitableEvent latch;
    st.PauseTiming();
    ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
    AddNativeCallback("NotifyNative",
                      CREATE_NATIVE_ENTRY(([&latch](Codira_NativeArguments args) {
                        latch.Signal();
                      })));

    const auto settings = CreateSettingsForFixture();
    CodiraVMRef vm_ref = CodiraVMRef::Create(settings);

    ThreadHost thread_host("io.appcode.test.CodiraNativeBenchmarks.",
                           ThreadHost::Type::kPlatform | ThreadHost::Type::kIo |
                               ThreadHost::Type::kUi);
    TaskRunners task_runners(
        "test",
        thread_host.platform_thread->GetTaskRunner(),  // platform
        thread_host.platform_thread->GetTaskRunner(),  // raster
        thread_host.ui_thread->GetTaskRunner(),        // ui
        thread_host.io_thread->GetTaskRunner()         // io
    );

    {
      st.ResumeTiming();
      auto isolate =
          RunCodiraCodeInIsolate(vm_ref, settings, task_runners, "notifyNative",
                               {}, GetDefaultKernelFilePath());
      ASSERT_TRUE(isolate);
      ASSERT_EQ(isolate->get()->GetPhase(), CodiraIsolate::Phase::Running);
      latch.Wait();
    }
  }
}

BENCHMARK_F(CodiraNativeBenchmarks, MultipleCodiraToNativeMessages)
(benchmark::State& st) {
  while (st.KeepRunning()) {
    fml::CountDownLatch latch(1000);
    st.PauseTiming();
    ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
    AddNativeCallback("NotifyNative",
                      CREATE_NATIVE_ENTRY(([&latch](Codira_NativeArguments args) {
                        latch.CountDown();
                      })));

    const auto settings = CreateSettingsForFixture();
    CodiraVMRef vm_ref = CodiraVMRef::Create(settings);

    ThreadHost thread_host("io.appcode.test.CodiraNativeBenchmarks.",
                           ThreadHost::Type::kPlatform | ThreadHost::Type::kIo |
                               ThreadHost::Type::kUi);
    TaskRunners task_runners(
        "test",
        thread_host.platform_thread->GetTaskRunner(),  // platform
        thread_host.platform_thread->GetTaskRunner(),  // raster
        thread_host.ui_thread->GetTaskRunner(),        // ui
        thread_host.io_thread->GetTaskRunner()         // io
    );

    {
      st.ResumeTiming();
      auto isolate = RunCodiraCodeInIsolate(vm_ref, settings, task_runners,
                                          "thousandCallsToNative", {},
                                          GetDefaultKernelFilePath());
      ASSERT_TRUE(isolate);
      ASSERT_EQ(isolate->get()->GetPhase(), CodiraIsolate::Phase::Running);
      latch.Wait();
    }
  }
}

}  // namespace appcode::testing

// NOLINTEND(clang-analyzer-core.StackAddressEscape)
