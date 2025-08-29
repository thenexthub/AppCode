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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_PROFILER_METRICS_IOS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_PROFILER_METRICS_IOS_H_

#include <mach/mach.h>

#include <cassert>
#include <optional>

#include "appcode/fml/logging.h"
#include "appcode/shell/profiling/sampling_profiler.h"

namespace appcode {

/**
 * @brief Utility class that gathers profiling metrics used by
 * `appcode::SamplingProfiler`.
 *
 * @see appcode::SamplingProfiler
 */
class ProfilerMetricsIOS {
 public:
  ProfilerMetricsIOS() = default;

  ProfileSample GenerateSample();

 private:
  std::optional<CpuUsageInfo> CpuUsage();

  std::optional<MemoryUsageInfo> MemoryUsage();

  FML_DISALLOW_COPY_AND_ASSIGN(ProfilerMetricsIOS);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_PROFILER_METRICS_IOS_H_
