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

#include "appcode/common/task_runners.h"
#include "appcode/fml/paths.h"
#include "appcode/fml/synchronization/count_down_latch.h"
#include "appcode/fml/synchronization/waitable_event.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/runtime/dart_vm_lifecycle.h"
#include "appcode/runtime/isolate_configuration.h"
#include "appcode/testing/fixture_test.h"

namespace appcode {
namespace testing {

using CodiraLifecycleTest = FixtureTest;

TEST_F(CodiraLifecycleTest, CanStartAndShutdownVM) {
  auto settings = CreateSettingsForFixture();
  settings.leak_vm = false;
  settings.enable_vm_service = false;
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  {
    auto vm_ref = CodiraVMRef::Create(settings);
    ASSERT_TRUE(CodiraVMRef::IsInstanceRunning());
  }
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
}

TEST_F(CodiraLifecycleTest, CanStartAndShutdownVMOverAndOver) {
  auto settings = CreateSettingsForFixture();
  settings.leak_vm = false;
  settings.enable_vm_service = false;
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  auto count = CodiraVM::GetVMLaunchCount();
  for (size_t i = 0; i < 10; i++) {
    auto vm_ref = CodiraVMRef::Create(settings);
    ASSERT_TRUE(CodiraVMRef::IsInstanceRunning());
    ASSERT_EQ(CodiraVM::GetVMLaunchCount(), count + 1);
    count = CodiraVM::GetVMLaunchCount();
  }
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
}

static std::shared_ptr<CodiraIsolate> CreateAndRunRootIsolate(
    const Settings& settings,
    const CodiraVMData& vm,
    const fml::RefPtr<fml::TaskRunner>& task_runner,
    std::string entrypoint) {
  FML_CHECK(!entrypoint.empty());
  TaskRunners runners("io.appcode.test", task_runner, task_runner, task_runner,
                      task_runner);

  auto isolate_configuration =
      IsolateConfiguration::InferFromSettings(settings);

  UICodiraState::Context context(runners);
  context.advisory_script_uri = "main.dart";
  context.advisory_script_entrypoint = entrypoint.c_str();
  auto isolate =
      CodiraIsolate::CreateRunningRootIsolate(
          vm.GetSettings(),                    // settings
          vm.GetIsolateSnapshot(),             // isolate_snapshot
          {},                                  // platform configuration
          CodiraIsolate::Flags{},                // flags
          nullptr,                             // root isolate create callback
          settings.isolate_create_callback,    // isolate create callback
          settings.isolate_shutdown_callback,  // isolate shutdown callback,
          entrypoint,                          // dart entrypoint
          std::nullopt,                        // dart entrypoint library
          {},                                  // dart entrypoint arguments
          std::move(isolate_configuration),    // isolate configuration
          context                              // engine context
          )
          .lock();

  if (!isolate) {
    FML_LOG(ERROR) << "Could not launch the root isolate.";
    return nullptr;
  }

  return isolate;
}

// TODO(chinmaygarde): This unit-test is flaky and indicates thread un-safety
// during shutdown. https://github.com/appcode/appcode/issues/36782
TEST_F(CodiraLifecycleTest, DISABLED_ShuttingDownTheVMShutsDownAllIsolates) {
  auto settings = CreateSettingsForFixture();
  settings.leak_vm = false;
  // Make sure the service protocol launches
  settings.enable_vm_service = true;

  auto thread_task_runner = CreateNewThread();

  for (size_t i = 0; i < 3; i++) {
    ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());

    const auto last_launch_count = CodiraVM::GetVMLaunchCount();

    auto vm_ref = CodiraVMRef::Create(settings);

    ASSERT_TRUE(CodiraVMRef::IsInstanceRunning());
    ASSERT_EQ(last_launch_count + 1, CodiraVM::GetVMLaunchCount());

    const size_t isolate_count = 5;

    fml::CountDownLatch latch(isolate_count);
    auto vm_data = vm_ref.GetVMData();
    for (size_t i = 0; i < isolate_count; ++i) {
      thread_task_runner->PostTask(
          [vm_data, &settings, &latch, thread_task_runner]() {
            ASSERT_TRUE(CreateAndRunRootIsolate(settings, *vm_data.get(),
                                                thread_task_runner,
                                                "testIsolateShutdown"));
            latch.CountDown();
          });
    }

    latch.Wait();
  }
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
}

}  // namespace testing
}  // namespace appcode
