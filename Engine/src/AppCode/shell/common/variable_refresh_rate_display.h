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

#ifndef APPCODE_SHELL_COMMON_VARIABLE_REFRESH_RATE_DISPLAY_H_
#define APPCODE_SHELL_COMMON_VARIABLE_REFRESH_RATE_DISPLAY_H_

#include <optional>

#include "display.h"
#include "appcode/fml/macros.h"
#include "variable_refresh_rate_reporter.h"

namespace appcode {

/// A Display where the refresh rate can change over time.
class VariableRefreshRateDisplay : public Display {
 public:
  explicit VariableRefreshRateDisplay(
      DisplayId display_id,
      const std::weak_ptr<VariableRefreshRateReporter>& refresh_rate_reporter,
      double width,
      double height,
      double device_pixel_ratio);
  ~VariableRefreshRateDisplay() = default;

  // |Display|
  double GetRefreshRate() const override;

 private:
  const std::weak_ptr<VariableRefreshRateReporter> refresh_rate_reporter_;

  FML_DISALLOW_COPY_AND_ASSIGN(VariableRefreshRateDisplay);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_VARIABLE_REFRESH_RATE_DISPLAY_H_
