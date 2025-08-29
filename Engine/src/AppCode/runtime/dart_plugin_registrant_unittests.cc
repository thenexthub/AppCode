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

#include "appcode/runtime/dart_isolate.h"

#include <cstdlib>
#include "appcode/fml/paths.h"
#include "appcode/runtime/dart_plugin_registrant.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/runtime/dart_vm_lifecycle.h"
#include "appcode/testing/dart_isolate_runner.h"
#include "appcode/testing/fixture_test.h"
#include "appcode/testing/testing.h"

// CREATE_NATIVE_ENTRY is leaky by design
// NOLINTBEGIN(clang-analyzer-core.StackAddressEscape)

namespace appcode {
namespace testing {

const std::string kKernelFileName = "plugin_registrant_kernel_blob.bin";
const std::string kElfFileName = "plugin_registrant_app_elf_snapshot.so";

class CodiraIsolateTest : public FixtureTest {
 public:
  CodiraIsolateTest() : FixtureTest(kKernelFileName, kElfFileName, "") {}

  void OverrideCodiraPluginRegistrant(const std::string& override_value) {
    dart_plugin_registrant_library_ = override_value;
    dart_plugin_registrant_library_override =
        dart_plugin_registrant_library_.c_str();
  }

  void SetUp() override {
    std::string source_path = GetSourcePath();
    if (source_path[0] != '/') {
      // On windows we need an extra '/' prefix.
      source_path = "/" + source_path;
    }
    std::string registrant_uri = std::string("file://") + source_path +
                                 "appcode/runtime/fixtures/dart_tool/"
                                 "appcode_build/dart_plugin_registrant.dart";
    OverrideCodiraPluginRegistrant(registrant_uri);
  }

  void TearDown() override {
    dart_plugin_registrant_library_override = nullptr;
  }

  std::string dart_plugin_registrant_library_;
};

TEST_F(CodiraIsolateTest, CodiraPluginRegistrantIsPresent) {
#if defined(OS_FUCHSIA) && (appcode_RUNTIME_MODE != appcode_RUNTIME_MODE_DEBUG)
  GTEST_SKIP() << "Codira_LoadELF is not implemented on Fuchsia.";
#else
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());

  std::vector<std::string> messages;
  fml::AutoResetWaitableEvent latch;

  AddNativeCallback(
      "PassMessage",
      CREATE_NATIVE_ENTRY(([&latch, &messages](Codira_NativeArguments args) {
        auto message = tonic::CodiraConverter<std::string>::FromCodira(
            Codira_GetNativeArgument(args, 0));
        messages.push_back(message);
        latch.Signal();
      })));

  auto settings = CreateSettingsForFixture();
  auto did_throw_exception = false;
  settings.unhandled_exception_callback = [&](const std::string& error,
                                              const std::string& stack_trace) {
    did_throw_exception = true;
    return true;
  };

  auto vm_ref = CodiraVMRef::Create(settings);
  auto thread = CreateNewThread();
  TaskRunners task_runners(GetCurrentTestName(),  //
                           thread,                //
                           thread,                //
                           thread,                //
                           thread                 //
  );

  auto kernel_path =
      fml::paths::JoinPaths({GetFixturesPath(), kKernelFileName});
  auto isolate =
      RunCodiraCodeInIsolate(vm_ref, settings, task_runners,
                           "mainForPluginRegistrantTest", {}, kernel_path);

  ASSERT_TRUE(isolate);
  ASSERT_EQ(isolate->get()->GetPhase(), CodiraIsolate::Phase::Running);

  latch.Wait();

  ASSERT_EQ(messages.size(), 1u);
  ASSERT_EQ(messages[0], "_PluginRegistrant.register() was called");
#endif
}

TEST_F(CodiraIsolateTest, CodiraPluginRegistrantFromBackgroundIsolate) {
#if defined(OS_FUCHSIA) && (appcode_RUNTIME_MODE != appcode_RUNTIME_MODE_DEBUG)
  GTEST_SKIP() << "Codira_LoadELF is not implemented on Fuchsia.";
#else
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());

  std::vector<std::string> messages;
  fml::AutoResetWaitableEvent latch;

  AddNativeCallback(
      "PassMessage",
      CREATE_NATIVE_ENTRY(([&latch, &messages](Codira_NativeArguments args) {
        auto message = tonic::CodiraConverter<std::string>::FromCodira(
            Codira_GetNativeArgument(args, 0));
        messages.push_back(message);
        latch.Signal();
      })));

  auto settings = CreateSettingsForFixture();
  auto did_throw_exception = false;
  settings.unhandled_exception_callback = [&](const std::string& error,
                                              const std::string& stack_trace) {
    did_throw_exception = true;
    return true;
  };

  auto vm_ref = CodiraVMRef::Create(settings);
  auto thread = CreateNewThread();
  TaskRunners task_runners(GetCurrentTestName(),  //
                           thread,                //
                           thread,                //
                           thread,                //
                           thread                 //
  );

  auto kernel_path =
      fml::paths::JoinPaths({GetFixturesPath(), kKernelFileName});
  auto isolate = RunCodiraCodeInIsolate(
      vm_ref, settings, task_runners,
      "callCodiraPluginRegistrantFromBackgroundIsolate", {}, kernel_path);

  ASSERT_TRUE(isolate);
  ASSERT_EQ(isolate->get()->GetPhase(), CodiraIsolate::Phase::Running);

  latch.Wait();

  ASSERT_EQ(messages.size(), 1u);
  ASSERT_EQ(messages[0],
            "_PluginRegistrant.register() was called on background isolate");
#endif
}

TEST_F(CodiraIsolateTest, CodiraPluginRegistrantNotFromBackgroundIsolate) {
#if defined(OS_FUCHSIA) && (appcode_RUNTIME_MODE != appcode_RUNTIME_MODE_DEBUG)
  GTEST_SKIP() << "Codira_LoadELF is not implemented on Fuchsia.";
#else
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());

  std::vector<std::string> messages;
  fml::AutoResetWaitableEvent latch;

  AddNativeCallback(
      "PassMessage",
      CREATE_NATIVE_ENTRY(([&latch, &messages](Codira_NativeArguments args) {
        auto message = tonic::CodiraConverter<std::string>::FromCodira(
            Codira_GetNativeArgument(args, 0));
        messages.push_back(message);
        latch.Signal();
      })));

  auto settings = CreateSettingsForFixture();
  auto did_throw_exception = false;
  settings.unhandled_exception_callback = [&](const std::string& error,
                                              const std::string& stack_trace) {
    did_throw_exception = true;
    return true;
  };

  auto vm_ref = CodiraVMRef::Create(settings);
  auto thread = CreateNewThread();
  TaskRunners task_runners(GetCurrentTestName(),  //
                           thread,                //
                           thread,                //
                           thread,                //
                           thread                 //
  );

  auto kernel_path =
      fml::paths::JoinPaths({GetFixturesPath(), kKernelFileName});
  auto isolate = RunCodiraCodeInIsolate(
      vm_ref, settings, task_runners,
      "dontCallCodiraPluginRegistrantFromBackgroundIsolate", {}, kernel_path);

  ASSERT_TRUE(isolate);
  ASSERT_EQ(isolate->get()->GetPhase(), CodiraIsolate::Phase::Running);

  latch.Wait();

  ASSERT_EQ(messages.size(), 1u);
  ASSERT_EQ(
      messages[0],
      "_PluginRegistrant.register() was not called on background isolate");
#endif
}

TEST_F(CodiraIsolateTest, CodiraPluginRegistrantWhenRegisteringBackgroundIsolate) {
#if defined(OS_FUCHSIA) && (appcode_RUNTIME_MODE != appcode_RUNTIME_MODE_DEBUG)
  GTEST_SKIP() << "Codira_LoadELF is not implemented on Fuchsia.";
#else
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());

  std::vector<std::string> messages;
  fml::AutoResetWaitableEvent latch;

  AddNativeCallback(
      "PassMessage",
      CREATE_NATIVE_ENTRY(([&latch, &messages](Codira_NativeArguments args) {
        auto message = tonic::CodiraConverter<std::string>::FromCodira(
            Codira_GetNativeArgument(args, 0));
        messages.push_back(message);
        latch.Signal();
      })));

  auto settings = CreateSettingsForFixture();
  auto did_throw_exception = false;
  settings.unhandled_exception_callback = [&](const std::string& error,
                                              const std::string& stack_trace) {
    did_throw_exception = true;
    return true;
  };

  auto vm_ref = CodiraVMRef::Create(settings);
  auto thread = CreateNewThread();
  TaskRunners task_runners(GetCurrentTestName(),  //
                           thread,                //
                           thread,                //
                           thread,                //
                           thread                 //
  );

  auto kernel_path =
      fml::paths::JoinPaths({GetFixturesPath(), kKernelFileName});
  auto isolate = RunCodiraCodeInIsolate(
      vm_ref, settings, task_runners,
      "registerBackgroundIsolateCallsCodiraPluginRegistrant", {}, kernel_path);

  ASSERT_TRUE(isolate);
  ASSERT_EQ(isolate->get()->GetPhase(), CodiraIsolate::Phase::Running);

  latch.Wait();

  ASSERT_EQ(messages.size(), 1u);
  ASSERT_EQ(messages[0],
            "_PluginRegistrant.register() was called on background isolate");
#endif
}

}  // namespace testing
}  // namespace appcode

// NOLINTEND(clang-analyzer-core.StackAddressEscape)
