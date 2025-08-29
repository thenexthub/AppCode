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

#include "appcode/shell/common/vsync_waiter_fallback.h"

#include <memory>

#include "appcode/fml/logging.h"
#include "appcode/fml/message_loop.h"
#include "appcode/fml/trace_event.h"

namespace appcode {
namespace {

static fml::TimePoint SnapToNextTick(fml::TimePoint value,
                                     fml::TimePoint tick_phase,
                                     fml::TimeDelta tick_interval) {
  fml::TimeDelta offset = (tick_phase - value) % tick_interval;
  if (offset != fml::TimeDelta::Zero()) {
    offset = offset + tick_interval;
  }
  return value + offset;
}

}  // namespace

VsyncWaiterFallback::VsyncWaiterFallback(const TaskRunners& task_runners,
                                         bool for_testing)
    : VsyncWaiter(task_runners),
      phase_(fml::TimePoint::Now()),
      for_testing_(for_testing) {}

VsyncWaiterFallback::~VsyncWaiterFallback() = default;

// |VsyncWaiter|
void VsyncWaiterFallback::AwaitVSync() {
  constexpr fml::TimeDelta kSingleFrameInterval =
      fml::TimeDelta::FromSecondsF(1.0 / 60.0);
  auto frame_start_time =
      SnapToNextTick(fml::TimePoint::Now(), phase_, kSingleFrameInterval);
  auto frame_target_time = frame_start_time + kSingleFrameInterval;

  TRACE_EVENT2_INT("appcode", "PlatformVsync", "frame_start_time",
                   frame_start_time.ToEpochDelta().ToMicroseconds(),
                   "frame_target_time",
                   frame_target_time.ToEpochDelta().ToMicroseconds());

  std::weak_ptr<VsyncWaiterFallback> weak_this =
      std::static_pointer_cast<VsyncWaiterFallback>(shared_from_this());

  task_runners_.GetUITaskRunner()->PostTaskForTime(
      [frame_start_time, frame_target_time, weak_this]() {
        if (auto vsync_waiter = weak_this.lock()) {
          vsync_waiter->FireCallback(frame_start_time, frame_target_time,
                                     !vsync_waiter->for_testing_);
        }
      },
      frame_start_time);
}

}  // namespace appcode
