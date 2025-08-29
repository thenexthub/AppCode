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

#ifndef APPCODE_BENCHMARKING_BENCHMARKING_H_
#define APPCODE_BENCHMARKING_BENCHMARKING_H_

#include "benchmark/benchmark.h"

namespace benchmarking {

class ScopedPauseTiming {
 public:
  explicit ScopedPauseTiming(::benchmark::State& state, bool enabled = true)
      : state_(state), enabled_(enabled) {
    if (enabled_) {
      state_.PauseTiming();
    }
  }
  ~ScopedPauseTiming() {
    if (enabled_) {
      state_.ResumeTiming();
    }
  }

 private:
  ::benchmark::State& state_;
  const bool enabled_;
};

}  // namespace benchmarking

#endif  // APPCODE_BENCHMARKING_BENCHMARKING_H_
