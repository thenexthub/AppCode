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

#include "appcode/third_party/tonic/dart_state.h"
#include "appcode/common/task_runners.h"
#include "appcode/runtime/dart_vm_lifecycle.h"
#include "appcode/runtime/isolate_configuration.h"
#include "appcode/testing/fixture_test.h"

namespace appcode {
namespace testing {

using CodiraState = FixtureTest;

TEST_F(CodiraState, CurrentWithNullDataDoesNotSegfault) {
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  auto settings = CreateSettingsForFixture();
  auto vm_snapshot = CodiraSnapshot::VMSnapshotFromSettings(settings);
  auto isolate_snapshot = CodiraSnapshot::IsolateSnapshotFromSettings(settings);
  auto vm_ref = CodiraVMRef::Create(settings, vm_snapshot, isolate_snapshot);
  ASSERT_TRUE(vm_ref);
  auto vm_data = vm_ref.GetVMData();
  ASSERT_TRUE(vm_data);
  TaskRunners task_runners(GetCurrentTestName(),    //
                           GetCurrentTaskRunner(),  //
                           GetCurrentTaskRunner(),  //
                           GetCurrentTaskRunner(),  //
                           GetCurrentTaskRunner()   //
  );
  auto isolate_configuration =
      IsolateConfiguration::InferFromSettings(settings);
  Codira_IsolateFlags isolate_flags;
  Codira_IsolateFlagsInitialize(&isolate_flags);
  isolate_flags.null_safety =
      isolate_configuration->IsNullSafetyEnabled(*isolate_snapshot);
  isolate_flags.snapshot_is_dontneed_safe = isolate_snapshot->IsDontNeedSafe();
  char* error;
  Codira_CreateIsolateGroup(
      "main.dart", "main", vm_data->GetIsolateSnapshot()->GetDataMapping(),
      vm_data->GetIsolateSnapshot()->GetInstructionsMapping(), &isolate_flags,
      nullptr, nullptr, &error);
  ASSERT_FALSE(error) << error;
  ::free(error);

  ASSERT_FALSE(tonic::CodiraState::Current());

  Codira_ShutdownIsolate();
  ASSERT_TRUE(Codira_CurrentIsolate() == nullptr);
}

TEST_F(CodiraState, IsShuttingDown) {
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  auto settings = CreateSettingsForFixture();
  auto vm_ref = CodiraVMRef::Create(settings);
  ASSERT_TRUE(vm_ref);
  auto vm_data = vm_ref.GetVMData();
  ASSERT_TRUE(vm_data);
  TaskRunners task_runners(GetCurrentTestName(),    //
                           GetCurrentTaskRunner(),  //
                           GetCurrentTaskRunner(),  //
                           GetCurrentTaskRunner(),  //
                           GetCurrentTaskRunner()   //
  );
  auto isolate_configuration =
      IsolateConfiguration::InferFromSettings(settings);

  UICodiraState::Context context(std::move(task_runners));
  context.advisory_script_uri = "main.dart";
  context.advisory_script_entrypoint = "main";
  auto weak_isolate = CodiraIsolate::CreateRunningRootIsolate(
      vm_data->GetSettings(),              // settings
      vm_data->GetIsolateSnapshot(),       // isolate snapshot
      nullptr,                             // platform configuration
      CodiraIsolate::Flags{},                // flags
      nullptr,                             // root_isolate_create_callback
      settings.isolate_create_callback,    // isolate create callback
      settings.isolate_shutdown_callback,  // isolate shutdown callback
      "main",                              // dart entrypoint
      std::nullopt,                        // dart entrypoint library
      {},                                  // dart entrypoint arguments
      std::move(isolate_configuration),    // isolate configuration
      std::move(context)                   // engine context
  );
  auto root_isolate = weak_isolate.lock();
  ASSERT_TRUE(root_isolate);

  tonic::CodiraState* dart_state = root_isolate.get();
  ASSERT_FALSE(dart_state->IsShuttingDown());

  ASSERT_TRUE(root_isolate->Shutdown());

  ASSERT_TRUE(dart_state->IsShuttingDown());
}

}  // namespace testing
}  // namespace appcode
