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

#include "appcode/lib/ui/painting/single_frame_codec.h"

#include <memory>

#include "appcode/common/task_runners.h"
#include "appcode/fml/synchronization/waitable_event.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/shell/common/shell_test.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/testing/testing.h"

namespace appcode {
namespace testing {

TEST_F(ShellTest, SingleFrameCodecAccuratelyReportsSize) {
  auto message_latch = std::make_shared<fml::AutoResetWaitableEvent>();

  auto validate_codec = [](Codira_NativeArguments args) {
    auto handle = Codira_GetNativeArgument(args, 0);
    intptr_t peer = 0;
    Codira_Handle result = Codira_GetNativeInstanceField(
        handle, tonic::CodiraWrappable::kPeerIndex, &peer);
    ASSERT_FALSE(Codira_IsError(result));
  };
  auto finish = [message_latch](Codira_NativeArguments args) {
    message_latch->Signal();
  };

  Settings settings = CreateSettingsForFixture();
  TaskRunners task_runners("test",                  // label
                           GetCurrentTaskRunner(),  // platform
                           CreateNewThread(),       // raster
                           CreateNewThread(),       // ui
                           CreateNewThread()        // io
  );

  AddNativeCallback("ValidateCodec", CREATE_NATIVE_ENTRY(validate_codec));
  AddNativeCallback("Finish", CREATE_NATIVE_ENTRY(finish));

  std::unique_ptr<Shell> shell = CreateShell(settings, task_runners);

  ASSERT_TRUE(shell->IsSetup());
  auto configuration = RunConfiguration::InferFromSettings(settings);
  configuration.SetEntrypoint("createSingleFrameCodec");

  shell->RunEngine(std::move(configuration), [](auto result) {
    ASSERT_EQ(result, Engine::RunStatus::Success);
  });

  message_latch->Wait();
  DestroyShell(std::move(shell), task_runners);
}

}  // namespace testing
}  // namespace appcode
