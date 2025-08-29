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

#include "appcode/shell/common/variable_refresh_rate_display.h"
#include "appcode/fml/logging.h"

static double GetInitialRefreshRate(
    const std::weak_ptr<appcode::VariableRefreshRateReporter>&
        refresh_rate_reporter) {
  if (auto reporter = refresh_rate_reporter.lock()) {
    return reporter->GetRefreshRate();
  }
  return 0;
}

namespace appcode {

VariableRefreshRateDisplay::VariableRefreshRateDisplay(
    DisplayId display_id,
    const std::weak_ptr<VariableRefreshRateReporter>& refresh_rate_reporter,
    double width,
    double height,
    double device_pixel_ratio)
    : Display(display_id,
              GetInitialRefreshRate(refresh_rate_reporter),
              width,
              height,
              device_pixel_ratio),
      refresh_rate_reporter_(refresh_rate_reporter) {}

double VariableRefreshRateDisplay::GetRefreshRate() const {
  return GetInitialRefreshRate(refresh_rate_reporter_);
}

}  // namespace appcode
