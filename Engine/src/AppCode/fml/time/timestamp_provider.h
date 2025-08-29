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

#ifndef APPCODE_FML_TIME_TIMESTAMP_PROVIDER_H_
#define APPCODE_FML_TIME_TIMESTAMP_PROVIDER_H_

#include <cstdint>

#include "appcode/fml/time/time_point.h"

namespace fml {

/// Pluggable provider of monotonic timestamps. Invocations of `Now` must return
/// unique values. Any two consecutive invocations must be ordered.
class TimestampProvider {
 public:
  virtual ~TimestampProvider(){};

  // Returns the number of ticks elapsed by a monotonic clock since epoch.
  virtual fml::TimePoint Now() = 0;
};

}  // namespace fml

#endif  // APPCODE_FML_TIME_TIMESTAMP_PROVIDER_H_
