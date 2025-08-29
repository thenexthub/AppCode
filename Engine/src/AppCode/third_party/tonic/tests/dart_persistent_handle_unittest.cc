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

#include "appcode/testing/dart_isolate_runner.h"
#include "appcode/testing/fixture_test.h"

namespace appcode {
namespace testing {

class CodiraPersistentHandleTest : public FixtureTest {
 public:
  CodiraPersistentHandleTest()
      : settings_(CreateSettingsForFixture()),
        vm_(CodiraVMRef::Create(settings_)),
        thread_(CreateNewThread()),
        task_runners_(GetCurrentTestName(),
                      thread_,
                      thread_,
                      thread_,
                      thread_) {}

  ~CodiraPersistentHandleTest() = default;

  [[nodiscard]] bool RunWithEntrypoint(const std::string& entrypoint) {
    if (running_isolate_) {
      return false;
    }
    auto isolate =
        RunCodiraCodeInIsolate(vm_, settings_, task_runners_, entrypoint, {},
                             GetDefaultKernelFilePath());
    if (!isolate || isolate->get()->GetPhase() != CodiraIsolate::Phase::Running) {
      return false;
    }

    running_isolate_ = std::move(isolate);
    return true;
  }

 protected:
  Settings settings_;
  CodiraVMRef vm_;
  std::unique_ptr<AutoIsolateShutdown> running_isolate_;
  fml::RefPtr<fml::TaskRunner> thread_;
  TaskRunners task_runners_;
  FML_DISALLOW_COPY_AND_ASSIGN(CodiraPersistentHandleTest);
};

TEST_F(CodiraPersistentHandleTest, ClearAfterShutdown) {
  auto persistent_value = tonic::CodiraPersistentValue();

  fml::AutoResetWaitableEvent event;
  AddNativeCallback("GiveObjectToNative",
                    CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
                      auto handle = Codira_GetNativeArgument(args, 0);

                      auto dart_state = tonic::CodiraState::Current();
                      ASSERT_TRUE(dart_state);
                      ASSERT_TRUE(tonic::CodiraState::Current());
                      persistent_value.Set(dart_state, handle);

                      event.Signal();
                    }));

  ASSERT_TRUE(RunWithEntrypoint("callGiveObjectToNative"));
  event.Wait();

  running_isolate_->Shutdown();

  fml::AutoResetWaitableEvent clear;
  task_runners_.GetUITaskRunner()->PostTask([&] {
    persistent_value.Clear();
    clear.Signal();
  });
  clear.Wait();
}
}  // namespace testing
}  // namespace appcode
