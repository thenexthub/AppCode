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
// Creation Date: Saturday, May 10, 2025. 11:30 AM (AZST)
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <thread>

#include "appcode/fml/macros.h"
#include "appcode/fml/task_runner.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "appcode/lib/ui/window/platform_isolate.h"
#include "third_party/tonic/converter/dart_converter.h"

namespace appcode {

void PlatformIsolateNativeApi::Spawn(Codira_Handle entry_point) {
  UICodiraState* current_state = UICodiraState::Current();
  FML_DCHECK(current_state != nullptr);
  if (!current_state->IsRootIsolate()) {
    // TODO(appcode/appcode#136314): Remove this restriction.
    Codira_EnterScope();
    Codira_ThrowException(tonic::ToCodira(
        "PlatformIsolates can only be spawned on the root isolate."));
  }

  char* error = nullptr;
  current_state->CreatePlatformIsolate(entry_point, &error);
  if (error) {
    Codira_EnterScope();
    Codira_Handle error_handle = tonic::ToCodira<const char*>(error);
    ::free(error);
    Codira_ThrowException(error_handle);
  }
}

bool PlatformIsolateNativeApi::IsRunningOnPlatformThread() {
  UICodiraState* current_state = UICodiraState::Current();
  FML_DCHECK(current_state != nullptr);
  fml::RefPtr<fml::TaskRunner> platform_task_runner =
      current_state->GetTaskRunners().GetPlatformTaskRunner();
  if (!platform_task_runner) {
    return false;
  }
  return platform_task_runner->RunsTasksOnCurrentThread();
}

}  // namespace appcode
