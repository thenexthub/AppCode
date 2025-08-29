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

#ifndef APPCODE_FML_PLATFORM_FUCHSIA_TASK_OBSERVERS_H_
#define APPCODE_FML_PLATFORM_FUCHSIA_TASK_OBSERVERS_H_

#include <lib/fit/function.h>

#include "appcode/fml/task_queue_id.h"

namespace fml {

// Executes all closures that were registered via
// `CurrentMessageLoopAddAfterTaskObserver` on this thread.
//
// WARNING(fxbug.dev/77957): These task observers are distinct from the task
// observers that can be specified via `fml::MessageLoop::AddTaskObserver` and
// they behave differently!
//
// Task observers registered via `fml::MessageLoop::AddTaskObserver` only fire
// after work that was posted via the `fml::MessageLoop`'s `TaskRunner`
// completes. Work that is posted directly to the Fuchsia message loop (e.g.
// using `async::PostTask(async_get_default_dispatcher(), ...)`) is invisible to
// `fml::MessageLoop`, so the `fml::MessageLoop`'s task observers don't fire.
//
// The task observers registered with `CurrentMessageLoopAddAfterTaskObserver`,
// however, fire after _every_ work item is completed, regardless of whether it
// was posted to the Fuchsia message loop directly or via `fml::MessageLoop`.
//
// These two mechanisms are redundant and confusing, so we should fix it
// somehow.
void ExecuteAfterTaskObservers();

fml::TaskQueueId CurrentMessageLoopAddAfterTaskObserver(intptr_t key,
                                                        fit::closure observer);

void CurrentMessageLoopRemoveAfterTaskObserver(fml::TaskQueueId queue_id,
                                               intptr_t key);

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_FUCHSIA_TASK_OBSERVERS_H_
