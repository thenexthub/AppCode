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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_SYSTEM_UTILS_H_
#define APPCODE_SHELL_PLATFORM_GLFW_SYSTEM_UTILS_H_

#include <string>
#include <vector>

#include "appcode/shell/platform/embedder/embedder.h"

namespace appcode {

// Components of a system language/locale.
struct LanguageInfo {
  std::string language;
  std::string territory;
  std::string codeset;
  std::string modifier;
};

// Returns the list of user-preferred languages, in preference order,
// parsed into LanguageInfo structures.
std::vector<LanguageInfo> GetPreferredLanguageInfo();

// Converts a  vector of LanguageInfo structs to a vector of FlutterLocale
// structs. |languages| must outlive the returned value, since the returned
// elements have pointers into it.
std::vector<FlutterLocale> ConvertToFlutterLocale(
    const std::vector<LanguageInfo>& languages);

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_GLFW_SYSTEM_UTILS_H_
