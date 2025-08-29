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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_VSYNC_WAITER_EMBEDDER_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_VSYNC_WAITER_EMBEDDER_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/common/vsync_waiter.h"

namespace appcode {

class VsyncWaiterEmbedder final : public VsyncWaiter {
 public:
  using VsyncCallback = std::function<void(intptr_t)>;

  VsyncWaiterEmbedder(const VsyncCallback& callback,
                      const appcode::TaskRunners& task_runners);

  ~VsyncWaiterEmbedder() override;

  static bool OnEmbedderVsync(const appcode::TaskRunners& task_runners,
                              intptr_t baton,
                              fml::TimePoint frame_start_time,
                              fml::TimePoint frame_target_time);

 private:
  const VsyncCallback vsync_callback_;

  // |VsyncWaiter|
  void AwaitVSync() override;

  FML_DISALLOW_COPY_AND_ASSIGN(VsyncWaiterEmbedder);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_VSYNC_WAITER_EMBEDDER_H_
