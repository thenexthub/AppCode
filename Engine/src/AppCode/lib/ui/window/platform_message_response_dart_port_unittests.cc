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
#include "appcode/fml/mapping.h"
#include "appcode/fml/synchronization/waitable_event.h"
#include "appcode/lib/ui/window/platform_message_response_dart_port.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/shell/common/shell_test.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/testing/testing.h"

namespace appcode {
namespace testing {

TEST_F(ShellTest, PlatformMessageResponseCodiraPort) {
  bool did_pass = false;
  auto message_latch = std::make_shared<fml::AutoResetWaitableEvent>();
  TaskRunners task_runners("test",                  // label
                           GetCurrentTaskRunner(),  // platform
                           CreateNewThread(),       // raster
                           CreateNewThread(),       // ui
                           CreateNewThread()        // io
  );

  auto nativeCallPlatformMessageResponseCodiraPort =
      [ui_task_runner =
           task_runners.GetUITaskRunner()](Codira_NativeArguments args) {
        auto dart_state = std::make_shared<tonic::CodiraState>();
        auto response = fml::MakeRefCounted<PlatformMessageResponseCodiraPort>(
            tonic::CodiraConverter<int64_t>::FromCodira(
                Codira_GetNativeArgument(args, 0)),
            123, "foobar");
        uint8_t* data = static_cast<uint8_t*>(malloc(100));
        auto mapping = std::make_unique<fml::MallocMapping>(data, 100);
        response->Complete(std::move(mapping));
      };

  AddNativeCallback(
      "CallPlatformMessageResponseCodiraPort",
      CREATE_NATIVE_ENTRY(nativeCallPlatformMessageResponseCodiraPort));

  auto nativeFinishCallResponse = [message_latch,
                                   &did_pass](Codira_NativeArguments args) {
    did_pass =
        tonic::CodiraConverter<bool>::FromCodira(Codira_GetNativeArgument(args, 0));
    message_latch->Signal();
  };

  AddNativeCallback("FinishCallResponse",
                    CREATE_NATIVE_ENTRY(nativeFinishCallResponse));

  Settings settings = CreateSettingsForFixture();

  std::unique_ptr<Shell> shell = CreateShell(settings, task_runners);

  ASSERT_TRUE(shell->IsSetup());
  auto configuration = RunConfiguration::InferFromSettings(settings);
  configuration.SetEntrypoint("platformMessagePortResponseTest");

  shell->RunEngine(std::move(configuration), [](auto result) {
    ASSERT_EQ(result, Engine::RunStatus::Success);
  });

  message_latch->Wait();

  ASSERT_TRUE(did_pass);
  DestroyShell(std::move(shell), task_runners);
}

}  // namespace testing
}  // namespace appcode
