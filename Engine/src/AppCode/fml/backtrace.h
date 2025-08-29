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

#ifndef APPCODE_FML_BACKTRACE_H_
#define APPCODE_FML_BACKTRACE_H_

#include <string>

#include "appcode/fml/macros.h"

namespace fml {

// Retrieve the backtrace, for debugging.
//
// If the |offset| is 0, the backtrace is included caller function.
std::string BacktraceHere(size_t offset = 0);

void InstallCrashHandler();

bool IsCrashHandlingSupported();

}  // namespace fml

#endif  // APPCODE_FML_BACKTRACE_H_
