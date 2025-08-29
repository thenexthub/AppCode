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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_ENGINE_SWITCHES_H_
#define APPCODE_SHELL_PLATFORM_COMMON_ENGINE_SWITCHES_H_

#include <string>
#include <vector>

namespace appcode {

// Returns an array of engine switches suitable to pass to the embedder API
// in FlutterProjectArgs, based on parsing variables from the environment in
// the form:
//   APPCODE_ENGINE_SWITCHES=<count>
//   APPCODE_ENGINE_SWITCH_1=...
//   APPCODE_ENGINE_SWITCH_2=...
//   ...
// Values should match those in shell/common/switches.h
//
// The returned array does not include the initial dummy argument expected by
// the embedder API, so command_line_argv should not be set directly from it.
//
// In release mode, not all switches from the environment will necessarily be
// returned. See the implementation for details.
std::vector<std::string> GetSwitchesFromEnvironment();

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_ENGINE_SWITCHES_H_
