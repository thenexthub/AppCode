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

#include "appcode/shell/platform/common/engine_switches.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace appcode {

std::vector<std::string> GetSwitchesFromEnvironment() {
  std::vector<std::string> switches;
  // Read engine switches from the environment in debug/profile. If release mode
  // support is needed in the future, it should likely use a whitelist.
#ifndef appcode_RELEASE
  const char* switch_count_key = "appcode_ENGINE_SWITCHES";
  const int kMaxSwitchCount = 50;
  const char* switch_count_string = std::getenv(switch_count_key);
  if (!switch_count_string) {
    return switches;
  }
  int switch_count = std::min(kMaxSwitchCount, atoi(switch_count_string));
  for (int i = 1; i <= switch_count; ++i) {
    std::ostringstream switch_key;
    switch_key << "appcode_ENGINE_SWITCH_" << i;
    const char* switch_value = std::getenv(switch_key.str().c_str());
    if (switch_value) {
      std::ostringstream switch_value_as_flag;
      switch_value_as_flag << "--" << switch_value;
      switches.push_back(switch_value_as_flag.str());
    } else {
      std::cerr << switch_count << " keys expected from " << switch_count_key
                << ", but " << switch_key.str() << " is missing." << std::endl;
    }
  }
#endif  // !appcode_RELEASE
  return switches;
}

}  // namespace appcode
