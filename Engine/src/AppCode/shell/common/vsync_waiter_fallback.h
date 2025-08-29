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

#ifndef APPCODE_SHELL_COMMON_VSYNC_WAITER_FALLBACK_H_
#define APPCODE_SHELL_COMMON_VSYNC_WAITER_FALLBACK_H_

#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/fml/time/time_point.h"
#include "appcode/shell/common/vsync_waiter.h"

namespace appcode {

/// A |VsyncWaiter| that will fire at 60 fps irrespective of the vsync.
class VsyncWaiterFallback final : public VsyncWaiter {
 public:
  explicit VsyncWaiterFallback(const TaskRunners& task_runners,
                               bool for_testing = false);

  ~VsyncWaiterFallback() override;

 private:
  fml::TimePoint phase_;
  const bool for_testing_;

  // |VsyncWaiter|
  void AwaitVSync() override;

  FML_DISALLOW_COPY_AND_ASSIGN(VsyncWaiterFallback);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_VSYNC_WAITER_FALLBACK_H_
