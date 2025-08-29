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

#include "task_runner_adapter.h"

#include <lib/async/cpp/task.h>
#include <lib/async/default.h>
#include <lib/zx/time.h>

#include "appcode/fml/message_loop_impl.h"

namespace appcode_runner {

class CompatTaskRunner : public fml::TaskRunner {
 public:
  CompatTaskRunner(async_dispatcher_t* dispatcher)
      : fml::TaskRunner(nullptr), forwarding_target_(dispatcher) {
    FML_DCHECK(forwarding_target_);
  }

  void PostTask(const fml::closure& task) override {
    async::PostTask(forwarding_target_, task);
  }

  void PostTaskForTime(const fml::closure& task,
                       fml::TimePoint target_time) override {
    async::PostTaskForTime(
        forwarding_target_, task,
        zx::time(target_time.ToEpochDelta().ToNanoseconds()));
  }

  void PostDelayedTask(const fml::closure& task,
                       fml::TimeDelta delay) override {
    async::PostDelayedTask(forwarding_target_, task,
                           zx::duration(delay.ToNanoseconds()));
  }

  bool RunsTasksOnCurrentThread() override {
    return forwarding_target_ == async_get_default_dispatcher();
  }

 private:
  async_dispatcher_t* forwarding_target_;

  FML_DISALLOW_COPY_AND_ASSIGN(CompatTaskRunner);
  FML_FRIEND_MAKE_REF_COUNTED(CompatTaskRunner);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(CompatTaskRunner);
};

fml::RefPtr<fml::TaskRunner> CreateFMLTaskRunner(
    async_dispatcher_t* dispatcher) {
  return fml::MakeRefCounted<CompatTaskRunner>(dispatcher);
}

}  // namespace appcode_runner
