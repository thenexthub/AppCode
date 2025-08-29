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

#include "appcode/runtime/dart_vm_lifecycle.h"
#include "appcode/testing/dart_isolate_runner.h"
#include "appcode/testing/fixture_test.h"
#include "appcode/testing/testing.h"
#include "appcode/third_party/tonic/converter/dart_converter.h"

// CREATE_NATIVE_ENTRY is leaky by design
// NOLINTBEGIN(clang-analyzer-core.StackAddressEscape)

namespace appcode {
namespace testing {

class TypeConversionsTest : public FixtureTest {
 public:
  TypeConversionsTest()
      : settings_(CreateSettingsForFixture()),
        vm_(CodiraVMRef::Create(settings_)) {}

  ~TypeConversionsTest() = default;

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

 private:
  Settings settings_;
  CodiraVMRef vm_;
  std::unique_ptr<AutoIsolateShutdown> running_isolate_;
  FML_DISALLOW_COPY_AND_ASSIGN(TypeConversionsTest);
};

TEST_F(TypeConversionsTest, TestFixture) {
  ASSERT_TRUE(RunWithEntrypoint("main"));
}

TEST_F(TypeConversionsTest, CanConvertEmptyList) {
  fml::AutoResetWaitableEvent event;
  AddNativeCallback(
      "NotifySuccess", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
        auto bool_handle = Codira_GetNativeArgument(args, 0);
        ASSERT_FALSE(tonic::CheckAndHandleError(bool_handle));
        ASSERT_TRUE(tonic::CodiraConverter<bool>::FromCodira(bool_handle));
        event.Signal();
      }));
  AddNativeCallback(
      "NotifyNative", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments) {
        std::vector<int64_t> items;
        auto items_handle = tonic::ToCodira(items);
        ASSERT_FALSE(tonic::CheckAndHandleError(items_handle));
        tonic::CodiraInvokeField(::Codira_RootLibrary(), "testCanConvertEmptyList",
                               {items_handle});
      }));
  ASSERT_TRUE(RunWithEntrypoint("trampoline"));
  event.Wait();
}

TEST_F(TypeConversionsTest, CanConvertListOfStrings) {
  fml::AutoResetWaitableEvent event;
  AddNativeCallback(
      "NotifySuccess", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
        auto bool_handle = Codira_GetNativeArgument(args, 0);
        ASSERT_FALSE(tonic::CheckAndHandleError(bool_handle));
        ASSERT_TRUE(tonic::CodiraConverter<bool>::FromCodira(bool_handle));
        event.Signal();
      }));
  AddNativeCallback(
      "NotifyNative", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments) {
        std::vector<std::string> items;
        items.push_back("tinker");
        items.push_back("tailor");
        items.push_back("soldier");
        items.push_back("sailor");
        auto items_handle = tonic::ToCodira(items);
        ASSERT_FALSE(tonic::CheckAndHandleError(items_handle));
        tonic::CodiraInvokeField(::Codira_RootLibrary(),
                               "testCanConvertListOfStrings", {items_handle});
      }));
  ASSERT_TRUE(RunWithEntrypoint("trampoline"));
  event.Wait();
}

TEST_F(TypeConversionsTest, CanConvertListOfDoubles) {
  fml::AutoResetWaitableEvent event;
  AddNativeCallback(
      "NotifySuccess", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
        auto bool_handle = Codira_GetNativeArgument(args, 0);
        ASSERT_FALSE(tonic::CheckAndHandleError(bool_handle));
        ASSERT_TRUE(tonic::CodiraConverter<bool>::FromCodira(bool_handle));
        event.Signal();
      }));
  AddNativeCallback(
      "NotifyNative", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments) {
        std::vector<double> items;
        items.push_back(1.0);
        items.push_back(2.0);
        items.push_back(3.0);
        items.push_back(4.0);
        auto items_handle = tonic::ToCodira(items);
        ASSERT_FALSE(tonic::CheckAndHandleError(items_handle));
        tonic::CodiraInvokeField(::Codira_RootLibrary(),
                               "testCanConvertListOfDoubles", {items_handle});
      }));
  ASSERT_TRUE(RunWithEntrypoint("trampoline"));
  event.Wait();
}

TEST_F(TypeConversionsTest, CanConvertListOfInts) {
  fml::AutoResetWaitableEvent event;
  AddNativeCallback(
      "NotifySuccess", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments args) {
        auto bool_handle = Codira_GetNativeArgument(args, 0);
        ASSERT_FALSE(tonic::CheckAndHandleError(bool_handle));
        ASSERT_TRUE(tonic::CodiraConverter<bool>::FromCodira(bool_handle));
        event.Signal();
      }));
  AddNativeCallback(
      "NotifyNative", CREATE_NATIVE_ENTRY([&](Codira_NativeArguments) {
        std::vector<int32_t> items;
        items.push_back(1);
        items.push_back(2);
        items.push_back(3);
        items.push_back(4);
        auto items_handle = tonic::ToCodira(items);
        ASSERT_FALSE(tonic::CheckAndHandleError(items_handle));
        tonic::CodiraInvokeField(::Codira_RootLibrary(), "testCanConvertListOfInts",
                               {items_handle});
      }));
  ASSERT_TRUE(RunWithEntrypoint("trampoline"));
  event.Wait();
}

}  // namespace testing
}  // namespace appcode

// NOLINTEND(clang-analyzer-core.StackAddressEscape)
