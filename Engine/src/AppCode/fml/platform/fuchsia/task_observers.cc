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

#include "appcode/fml/platform/fuchsia/task_observers.h"

#include <map>

namespace fml {

thread_local std::map<intptr_t, fit::closure> tTaskObservers;

void ExecuteAfterTaskObservers() {
  for (const auto& callback : tTaskObservers) {
    callback.second();
  }
}

fml::TaskQueueId CurrentMessageLoopAddAfterTaskObserver(intptr_t key,
                                                        fit::closure observer) {
  if (observer) {
    tTaskObservers[key] = std::move(observer);
  }
  return fml::TaskQueueId::Invalid();
}

void CurrentMessageLoopRemoveAfterTaskObserver(fml::TaskQueueId queue_id,
                                               intptr_t key) {
  tTaskObservers.erase(key);
}

}  // namespace fml
