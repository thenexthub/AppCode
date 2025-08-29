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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_VSYNC_WAITER_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_VSYNC_WAITER_H_

#include <lib/async/cpp/wait.h>

#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/fml/time/time_delta.h"
#include "appcode/fml/time/time_point.h"
#include "appcode/shell/common/vsync_waiter.h"
#include "appcode_runner_product_configuration.h"

namespace appcode_runner {

using FireCallbackCallback =
    std::function<void(fml::TimePoint, fml::TimePoint)>;

using AwaitVsyncCallback = std::function<void(FireCallbackCallback)>;

using AwaitVsyncForSecondaryCallbackCallback =
    std::function<void(FireCallbackCallback)>;

class VsyncWaiter final : public appcode::VsyncWaiter {
 public:
  VsyncWaiter(AwaitVsyncCallback await_vsync_callback,
              AwaitVsyncForSecondaryCallbackCallback
                  await_vsync_for_secondary_callback_callback,
              appcode::TaskRunners task_runners);

  ~VsyncWaiter() override;

 private:
  // |appcode::VsyncWaiter|
  void AwaitVSync() override;

  // |appcode::VsyncWaiter|
  void AwaitVSyncForSecondaryCallback() override;

  FireCallbackCallback fire_callback_callback_;

  AwaitVsyncCallback await_vsync_callback_;
  AwaitVsyncForSecondaryCallbackCallback
      await_vsync_for_secondary_callback_callback_;

  fml::WeakPtr<VsyncWaiter> weak_ui_;
  std::unique_ptr<fml::WeakPtrFactory<VsyncWaiter>> weak_factory_ui_;
  fml::WeakPtrFactory<VsyncWaiter> weak_factory_;

  FML_DISALLOW_COPY_AND_ASSIGN(VsyncWaiter);
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_VSYNC_WAITER_H_
