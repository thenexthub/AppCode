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

#ifndef APPCODE_SHELL_COMMON_VSYNC_WAITERS_TEST_H_
#define APPCODE_SHELL_COMMON_VSYNC_WAITERS_TEST_H_

#define FML_USED_ON_EMBEDDER

#include <utility>

#include "appcode/shell/common/shell.h"

namespace appcode {
namespace testing {

using CreateVsyncWaiter = std::function<std::unique_ptr<VsyncWaiter>()>;

class ShellTestVsyncClock {
 public:
  /// Simulate that a vsync signal is triggered.
  void SimulateVSync();

  /// A future that will return the index the next vsync signal.
  std::future<int> NextVSync();

 private:
  std::mutex mutex_;
  std::vector<std::promise<int>> vsync_promised_;
  size_t vsync_issued_ = 0;
};

class ShellTestVsyncWaiter : public VsyncWaiter {
 public:
  ShellTestVsyncWaiter(const TaskRunners& task_runners,
                       std::shared_ptr<ShellTestVsyncClock> clock)
      : VsyncWaiter(task_runners), clock_(std::move(clock)) {}

 protected:
  void AwaitVSync() override;

 private:
  std::shared_ptr<ShellTestVsyncClock> clock_;
};

class ConstantFiringVsyncWaiter : public VsyncWaiter {
 public:
  // both of these are set in the past so as to fire immediately.
  static constexpr fml::TimePoint kFrameBeginTime =
      fml::TimePoint::FromEpochDelta(fml::TimeDelta::FromSeconds(0));
  static constexpr fml::TimePoint kFrameTargetTime =
      fml::TimePoint::FromEpochDelta(fml::TimeDelta::FromSeconds(100));

  explicit ConstantFiringVsyncWaiter(const TaskRunners& task_runners)
      : VsyncWaiter(task_runners) {}

 protected:
  void AwaitVSync() override;
};

class TestRefreshRateReporter final : public VariableRefreshRateReporter {
 public:
  explicit TestRefreshRateReporter(double refresh_rate);
  void UpdateRefreshRate(double refresh_rate);

  // |RefreshRateReporter|
  double GetRefreshRate() const override;

 private:
  double refresh_rate_;
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_VSYNC_WAITERS_TEST_H_
