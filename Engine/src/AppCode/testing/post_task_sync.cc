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

#include "appcode/testing/post_task_sync.h"

#include "appcode/fml/synchronization/waitable_event.h"

namespace appcode::testing {

void PostTaskSync(const fml::RefPtr<fml::TaskRunner>& task_runner,
                  const std::function<void()>& function) {
  fml::AutoResetWaitableEvent latch;
  task_runner->PostTask([&] {
    function();
    latch.Signal();
  });
  latch.Wait();
}

}  // namespace appcode::testing
