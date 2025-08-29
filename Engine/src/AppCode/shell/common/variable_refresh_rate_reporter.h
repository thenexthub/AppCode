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

#ifndef APPCODE_SHELL_COMMON_VARIABLE_REFRESH_RATE_REPORTER_H_
#define APPCODE_SHELL_COMMON_VARIABLE_REFRESH_RATE_REPORTER_H_

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace appcode {

/// Abstract class that reprents a platform specific mechanism to report current
/// refresh rates.
class VariableRefreshRateReporter {
 public:
  VariableRefreshRateReporter() = default;

  virtual double GetRefreshRate() const = 0;

  FML_DISALLOW_COPY_AND_ASSIGN(VariableRefreshRateReporter);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_VARIABLE_REFRESH_RATE_REPORTER_H_
