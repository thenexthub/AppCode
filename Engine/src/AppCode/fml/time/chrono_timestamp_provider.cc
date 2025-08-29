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

#include "appcode/fml/time/chrono_timestamp_provider.h"

#include <chrono>

namespace fml {

ChronoTimestampProvider::ChronoTimestampProvider() = default;

ChronoTimestampProvider::~ChronoTimestampProvider() = default;

fml::TimePoint ChronoTimestampProvider::Now() {
  const auto chrono_time_point = std::chrono::steady_clock::now();
  const auto ticks_since_epoch = chrono_time_point.time_since_epoch().count();
  return fml::TimePoint::FromTicks(ticks_since_epoch);
}

fml::TimePoint ChronoTicksSinceEpoch() {
  return ChronoTimestampProvider::Instance().Now();
}

}  // namespace fml
