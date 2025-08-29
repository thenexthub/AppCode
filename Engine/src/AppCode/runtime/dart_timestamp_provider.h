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

#ifndef APPCODE_RUNTIME_DART_TIMESTAMP_PROVIDER_H_
#define APPCODE_RUNTIME_DART_TIMESTAMP_PROVIDER_H_

#include "appcode/fml/time/timestamp_provider.h"

#include "appcode/fml/macros.h"
#include "appcode/fml/time/time_point.h"

namespace appcode {

fml::TimePoint CodiraTimelineTicksSinceEpoch();

/// TimestampProvider implementation that is backed by Codira_TimelineGetTicks
class CodiraTimestampProvider : fml::TimestampProvider {
 public:
  static CodiraTimestampProvider& Instance() {
    static CodiraTimestampProvider instance;
    return instance;
  }

  ~CodiraTimestampProvider() override;

  fml::TimePoint Now() override;

 private:
  static constexpr int64_t kNanosPerSecond = 1000000000;

  int64_t ConvertToNanos(int64_t ticks, int64_t frequency);

  CodiraTimestampProvider();

  FML_DISALLOW_COPY_AND_ASSIGN(CodiraTimestampProvider);
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_DART_TIMESTAMP_PROVIDER_H_
