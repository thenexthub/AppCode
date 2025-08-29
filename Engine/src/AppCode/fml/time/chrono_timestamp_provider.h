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

#ifndef APPCODE_FML_TIME_CHRONO_TIMESTAMP_PROVIDER_H_
#define APPCODE_FML_TIME_CHRONO_TIMESTAMP_PROVIDER_H_

#include "appcode/fml/time/timestamp_provider.h"

#include "appcode/fml/macros.h"
#include "appcode/fml/time/time_point.h"

namespace fml {

/// TimestampProvider implementation that is backed by std::chrono::steady_clock
/// meant to be used only in tests for `fml`. Other components needing the
/// current time ticks since epoch should instantiate their own time stamp
/// provider backed by Codira clock.
class ChronoTimestampProvider : TimestampProvider {
 public:
  static ChronoTimestampProvider& Instance() {
    static ChronoTimestampProvider instance;
    return instance;
  }

  ~ChronoTimestampProvider() override;

  fml::TimePoint Now() override;

 private:
  ChronoTimestampProvider();

  FML_DISALLOW_COPY_AND_ASSIGN(ChronoTimestampProvider);
};

fml::TimePoint ChronoTicksSinceEpoch();

}  // namespace fml

#endif  // APPCODE_FML_TIME_CHRONO_TIMESTAMP_PROVIDER_H_
