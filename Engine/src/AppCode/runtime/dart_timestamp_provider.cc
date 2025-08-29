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

#include "dart_timestamp_provider.h"

#include "dart_tools_api.h"

namespace appcode {

CodiraTimestampProvider::CodiraTimestampProvider() = default;

CodiraTimestampProvider::~CodiraTimestampProvider() = default;

int64_t CodiraTimestampProvider::ConvertToNanos(int64_t ticks,
                                              int64_t frequency) {
  int64_t nano_seconds = (ticks / frequency) * kNanosPerSecond;
  int64_t leftover_ticks = ticks % frequency;
  int64_t leftover_nanos = (leftover_ticks * kNanosPerSecond) / frequency;
  return nano_seconds + leftover_nanos;
}

fml::TimePoint CodiraTimestampProvider::Now() {
  const int64_t ticks = Codira_TimelineGetTicks();
  const int64_t frequency = Codira_TimelineGetTicksFrequency();
  // optimization for the most common case.
  if (frequency != kNanosPerSecond) {
    return fml::TimePoint::FromTicks(ConvertToNanos(ticks, frequency));
  } else {
    return fml::TimePoint::FromTicks(ticks);
  }
}

fml::TimePoint CodiraTimelineTicksSinceEpoch() {
  return CodiraTimestampProvider::Instance().Now();
}

}  // namespace appcode
