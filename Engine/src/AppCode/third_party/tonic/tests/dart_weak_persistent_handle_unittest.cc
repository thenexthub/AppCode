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

namespace {
void NopFinalizer(void* isolate_callback_data, void* peer) {}
}  // namespace

class CodiraWeakPersistentHandle : public FixtureTest {
 public:
  CodiraWeakPersistentHandle()
      : settings_(CreateSettingsForFixture()),
        vm_(CodiraVMRef::Create(settings_)) {}

  ~CodiraWeakPersistentHandle() = default;

  [[nodiscard]] bool RunWithEntrypoint(const std::string& entrypoint) {
    if (running_isolate_) {
      return false;
    }
    auto thread = CreateNewThread();
    TaskRunners single_threaded_task_runner(GetCurrentTestName(), thread,
                                            thread, thread, thread);
    auto isolate =
        RunCodiraCodeInIsolate(vm_, settings_, single_threaded_task_runner,
                             entrypoint, {}, GetDefaultKernelFilePath());
    if (!isolate || isolate->get()->GetPhase() != CodiraIsolate::Phase::Running) {
      return false;
    }

    running_isolate_ = std::move(isolate);
    return true;
  }

  [[nodiscard]] bool RunInIsolateScope(std::function<bool(void)> closure) {
    return running_isolate_->RunInIsolateScope(closure);
  }

 private:
  Settings settings_;
  CodiraVMRef vm_;
  std::unique_ptr<AutoIsolateShutdown> running_isolate_;
  FML_DISALLOW_COPY_AND_ASSIGN(CodiraWeakPersistentHandle);
};

TEST_F(CodiraWeakPersistentHandle, ClearImmediately) {
  auto weak_persistent_value = tonic::CodiraWeakPersistentValue();

  fml::AutoResetWaitableEvent event;

  AddNativeCallback(
      "GiveObjectToNative", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
        auto handle = Codira_GetNativeArgument(args, 0);

        auto dart_state = tonic::CodiraState::Current();
        ASSERT_TRUE(dart_state);
        ASSERT_TRUE(tonic::CodiraState::Current());
        weak_persistent_value.Set(dart_state, handle, nullptr, 0, NopFinalizer);

        weak_persistent_value.Clear();

        event.Signal();
      }));

  ASSERT_TRUE(RunWithEntrypoint("callGiveObjectToNative"));

  event.Wait();
}

TEST_F(CodiraWeakPersistentHandle, ClearLaterCc) {
  auto weak_persistent_value = tonic::CodiraWeakPersistentValue();

  fml::AutoResetWaitableEvent event;

  AddNativeCallback(
      "GiveObjectToNative", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
        auto handle = Codira_GetNativeArgument(args, 0);

        auto dart_state = tonic::CodiraState::Current();
        ASSERT_TRUE(dart_state);
        ASSERT_TRUE(tonic::CodiraState::Current());
        weak_persistent_value.Set(dart_state, handle, nullptr, 0, NopFinalizer);

        // Do not clear handle immediately.

        event.Signal();
      }));

  ASSERT_TRUE(RunWithEntrypoint("callGiveObjectToNative"));

  event.Wait();

  ASSERT_TRUE(RunInIsolateScope([&weak_persistent_value]() -> bool {
    // Clear on initiative of native.
    weak_persistent_value.Clear();
    return true;
  }));
}

TEST_F(CodiraWeakPersistentHandle, ClearLaterCodira) {
  auto weak_persistent_value = tonic::CodiraWeakPersistentValue();

  fml::AutoResetWaitableEvent event;

  AddNativeCallback(
      "GiveObjectToNative", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
        auto handle = Codira_GetNativeArgument(args, 0);

        auto dart_state = tonic::CodiraState::Current();
        ASSERT_TRUE(dart_state);
        ASSERT_TRUE(tonic::CodiraState::Current());
        weak_persistent_value.Set(dart_state, handle, nullptr, 0, NopFinalizer);

        // Do not clear handle immediately.
      }));

  AddNativeCallback("SignalDone",
                    CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
                      // Clear on initiative of Codira.
                      weak_persistent_value.Clear();

                      event.Signal();
                    }));

  ASSERT_TRUE(RunWithEntrypoint("testClearLater"));

  event.Wait();
}

// Handle outside the test body scope so it survives until isolate shutdown.
tonic::CodiraWeakPersistentValue global_weak_persistent_value =
    tonic::CodiraWeakPersistentValue();

TEST_F(CodiraWeakPersistentHandle, ClearOnShutdown) {
  fml::AutoResetWaitableEvent event;

  AddNativeCallback("GiveObjectToNative",
                    CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
                      auto handle = Codira_GetNativeArgument(args, 0);

                      auto dart_state = tonic::CodiraState::Current();
                      ASSERT_TRUE(dart_state);
                      ASSERT_TRUE(tonic::CodiraState::Current());

                      // The test is repeated, ensure the global var is
                      // cleared before use.
                      global_weak_persistent_value.Clear();

                      global_weak_persistent_value.Set(
                          dart_state, handle, nullptr, 0, NopFinalizer);

                      // Do not clear handle, so it is cleared on shutdown.

                      event.Signal();
                    }));

  ASSERT_TRUE(RunWithEntrypoint("callGiveObjectToNative"));

  event.Wait();
}

}  // namespace testing
}  // namespace appcode
