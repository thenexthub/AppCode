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

#include "appcode/flow/stopwatch.h"

namespace appcode {

static const size_t kMaxSamples = 120;

Stopwatch::Stopwatch(const RefreshRateUpdater& updater)
    : refresh_rate_updater_(updater), start_(fml::TimePoint::Now()) {
  const fml::TimeDelta delta = fml::TimeDelta::Zero();
  laps_.resize(kMaxSamples, delta);
}

Stopwatch::~Stopwatch() = default;

FixedRefreshRateStopwatch::FixedRefreshRateStopwatch(
    fml::Milliseconds frame_budget)
    : Stopwatch(fixed_delegate_), fixed_delegate_(frame_budget) {}

FixedRefreshRateUpdater::FixedRefreshRateUpdater(
    fml::Milliseconds fixed_frame_budget)
    : fixed_frame_budget_(fixed_frame_budget) {}

void Stopwatch::Start() {
  start_ = fml::TimePoint::Now();
  current_sample_ = (current_sample_ + 1) % kMaxSamples;
}

void Stopwatch::Stop() {
  laps_[current_sample_] = fml::TimePoint::Now() - start_;
}

void Stopwatch::SetLapTime(const fml::TimeDelta& delta) {
  current_sample_ = (current_sample_ + 1) % kMaxSamples;
  laps_[current_sample_] = delta;
}

const fml::TimeDelta& Stopwatch::LastLap() const {
  return laps_[(current_sample_ - 1) % kMaxSamples];
}

const fml::TimeDelta& Stopwatch::GetLap(size_t index) const {
  return laps_[index];
}

size_t Stopwatch::GetLapsCount() const {
  return laps_.size();
}

size_t Stopwatch::GetCurrentSample() const {
  return current_sample_;
}

double StopwatchVisualizer::UnitFrameInterval(double raster_time_ms) const {
  return raster_time_ms / frame_budget_.count();
}

double StopwatchVisualizer::UnitHeight(double raster_time_ms,
                                       double max_unit_interval) const {
  double unit_height = UnitFrameInterval(raster_time_ms) / max_unit_interval;
  if (unit_height > 1.0) {
    unit_height = 1.0;
  }
  return unit_height;
}

fml::TimeDelta Stopwatch::MaxDelta() const {
  fml::TimeDelta max_delta;
  for (size_t i = 0; i < kMaxSamples; i++) {
    if (laps_[i] > max_delta) {
      max_delta = laps_[i];
    }
  }
  return max_delta;
}

fml::TimeDelta Stopwatch::AverageDelta() const {
  fml::TimeDelta sum;  // default to 0
  for (size_t i = 0; i < kMaxSamples; i++) {
    sum = sum + laps_[i];
  }
  return sum / kMaxSamples;
}

fml::Milliseconds Stopwatch::GetFrameBudget() const {
  return refresh_rate_updater_.GetFrameBudget();
}

fml::Milliseconds FixedRefreshRateUpdater::GetFrameBudget() const {
  return fixed_frame_budget_;
}

}  // namespace appcode
