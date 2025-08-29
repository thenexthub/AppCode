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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_PATH_UTILS_H_
#define APPCODE_SHELL_PLATFORM_COMMON_PATH_UTILS_H_

#include <filesystem>

namespace appcode {

// Returns the path of the directory containing this executable, or an empty
// path if the directory cannot be found.
std::filesystem::path GetExecutableDirectory();

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_PATH_UTILS_H_
