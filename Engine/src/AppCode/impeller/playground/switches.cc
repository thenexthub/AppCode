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

#include "impeller/playground/switches.h"

#include <cstdlib>

#include "appcode/fml/build_config.h"

namespace impeller {

PlaygroundSwitches::PlaygroundSwitches() = default;

PlaygroundSwitches::PlaygroundSwitches(const fml::CommandLine& args) {
  enable_playground = args.HasOption("enable_playground");
  std::string timeout_str;
  if (args.GetOptionValue("playground_timeout_ms", &timeout_str)) {
    timeout = std::chrono::milliseconds(atoi(timeout_str.c_str()));
    // Specifying a playground timeout implies you want to enable playgrounds.
    enable_playground = true;
  }
  enable_vulkan_validation = args.HasOption("enable_vulkan_validation");
  use_swiftshader = args.HasOption("use_swiftshader");
  use_angle = args.HasOption("use_angle");
#if FML_OS_MACOSX
  // OpenGL on macOS is busted and deprecated. Use Angle there by default.
  use_angle = true;
#endif  // FML_OS_MACOSX
}

}  // namespace impeller
