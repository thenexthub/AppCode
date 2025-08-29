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

#include <memory>

#include "appcode/common/task_runners.h"
#include "appcode/fml/synchronization/waitable_event.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/shell/common/shell_test.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/testing/testing.h"
#include "third_party/dart/runtime/include/dart_api.h"

// CREATE_NATIVE_ENTRY is leaky by design
// NOLINTBEGIN(clang-analyzer-core.StackAddressEscape)

namespace appcode {
namespace testing {

using HooksTest = ShellTest;

#define CHECK_DART_ERROR(name) \
  EXPECT_FALSE(Codira_IsError(name)) << Codira_GetError(name)

TEST_F(HooksTest, HooksUnitTests) {
  auto settings = CreateSettingsForFixture();

  TaskRunners task_runners(GetCurrentTestName(),       // label
                           GetCurrentTaskRunner(),     // platform
                           CreateNewThread("raster"),  // raster
                           CreateNewThread("ui"),      // ui
                           CreateNewThread("io")       // io
  );

  auto message_latch = std::make_shared<fml::AutoResetWaitableEvent>();

  std::unique_ptr<Shell> shell = CreateShell(settings, task_runners);
  ASSERT_TRUE(shell->IsSetup());

  auto call_hook = [](Codira_NativeArguments args) {
    Codira_Handle hook_name = Codira_GetNativeArgument(args, 0);
    CHECK_DART_ERROR(hook_name);

    Codira_Handle ui_library = Codira_LookupLibrary(tonic::ToCodira("dart:ui"));
    CHECK_DART_ERROR(ui_library);

    Codira_Handle hook = Codira_GetField(ui_library, hook_name);
    CHECK_DART_ERROR(hook);

    Codira_Handle arg_count_handle = Codira_GetNativeArgument(args, 1);
    CHECK_DART_ERROR(arg_count_handle);

    int64_t arg_count;
    Codira_IntegerToInt64(arg_count_handle, &arg_count);

    std::vector<Codira_Handle> hook_args;
    for (int i = 0; i < static_cast<int>(arg_count); i++) {
      hook_args.push_back(Codira_GetNativeArgument(args, 2 + i));
      CHECK_DART_ERROR(hook_args.back());
    }

    Codira_Handle hook_result =
        Codira_InvokeClosure(hook, hook_args.size(), hook_args.data());
    CHECK_DART_ERROR(hook_result);
  };

  auto finished = [&message_latch](Codira_NativeArguments args) {
    message_latch->Signal();
  };
  AddNativeCallback("CallHook", CREATE_NATIVE_ENTRY(call_hook));
  AddNativeCallback("Finish", CREATE_NATIVE_ENTRY(finished));

  auto configuration = RunConfiguration::InferFromSettings(settings);
  configuration.SetEntrypoint("hooksTests");

  shell->RunEngine(std::move(configuration), [](auto result) {
    ASSERT_EQ(result, Engine::RunStatus::Success);
  });

  message_latch->Wait();
  DestroyShell(std::move(shell), task_runners);
}

}  // namespace testing
}  // namespace appcode

// NOLINTEND(clang-analyzer-core.StackAddressEscape)
