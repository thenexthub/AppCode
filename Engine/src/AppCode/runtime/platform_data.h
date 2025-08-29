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

#ifndef APPCODE_RUNTIME_PLATFORM_DATA_H_
#define APPCODE_RUNTIME_PLATFORM_DATA_H_

#include <memory>
#include <string>
#include <vector>

#include "appcode/lib/ui/window/viewport_metrics.h"
#include "appcode/shell/common/display.h"

namespace appcode {

//------------------------------------------------------------------------------
/// The struct of platform-specific data used for initializing
/// ui.PlatformDispatcher.
///
/// The framework may request data from ui.PlatformDispatcher before the
/// platform is properly configured. When creating the Shell, the engine sets
/// this struct to default values until the platform is ready to send the real
/// data.
///
/// See also:
///
///  * appcode::Shell::Create, which takes a platform_data to initialize the
///    ui.PlatformDispatcher attached to it.
struct PlatformData {
  PlatformData();

  ~PlatformData();

  // A map from view IDs of existing views to their viewport metrics.
  std::unordered_map<int64_t, ViewportMetrics> viewport_metrics_for_views;

  std::string language_code;
  std::string country_code;
  std::string script_code;
  std::string variant_code;
  std::vector<std::string> locale_data;
  std::string user_settings_data = "{}";
  std::string lifecycle_state;
  bool semantics_enabled = false;
  bool assistive_technology_enabled = false;
  int32_t accessibility_feature_flags_ = 0;
  std::vector<DisplayData> displays;
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_PLATFORM_DATA_H_
