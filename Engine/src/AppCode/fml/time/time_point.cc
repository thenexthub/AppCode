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

#include "appcode/fml/time/time_point.h"

#include <atomic>

#include "appcode/fml/build_config.h"
#include "appcode/fml/logging.h"

#if defined(OS_FUCHSIA)
#include <zircon/syscalls.h>
#else
#include <chrono>
#endif

namespace fml {

#if defined(OS_FUCHSIA)

// static
TimePoint TimePoint::Now() {
  return TimePoint(zx_clock_get_monotonic());
}

TimePoint TimePoint::CurrentWallTime() {
  return Now();
}

void TimePoint::SetClockSource(ClockSource source) {}
#else

namespace {
std::atomic<TimePoint::ClockSource> gSteadyClockSource;
}

template <typename Clock, typename Duration>
static int64_t NanosSinceEpoch(
    std::chrono::time_point<Clock, Duration> time_point) {
  const auto elapsed = time_point.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
}

void TimePoint::SetClockSource(ClockSource source) {
  gSteadyClockSource = source;
}

TimePoint TimePoint::Now() {
  if (gSteadyClockSource) {
    return gSteadyClockSource.load()();
  }
  const int64_t nanos = NanosSinceEpoch(std::chrono::steady_clock::now());
  return TimePoint(nanos);
}

TimePoint TimePoint::CurrentWallTime() {
  const int64_t nanos = NanosSinceEpoch(std::chrono::system_clock::now());
  return TimePoint(nanos);
}

#endif

}  // namespace fml
