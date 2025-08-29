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

#include "appcode/runtime/dart_timestamp_provider.h"

#include <thread>

#include "gtest/gtest.h"

namespace fml {
namespace {

TEST(TimePoint, Control) {
  EXPECT_LT(TimePoint::Min(), ChronoTicksSinceEpoch());
  EXPECT_GT(TimePoint::Max(), ChronoTicksSinceEpoch());
}

TEST(TimePoint, CodiraClockIsMonotonic) {
  using namespace std::chrono_literals;
  const auto t1 = appcode::CodiraTimelineTicksSinceEpoch();
  std::this_thread::sleep_for(1us);
  const auto t2 = appcode::CodiraTimelineTicksSinceEpoch();
  std::this_thread::sleep_for(1us);
  const auto t3 = appcode::CodiraTimelineTicksSinceEpoch();
  EXPECT_LT(TimePoint::Min(), t1);
  EXPECT_LE(t1, t2);
  EXPECT_LE(t2, t3);
  EXPECT_LT(t3, TimePoint::Max());
}

}  // namespace
}  // namespace fml
