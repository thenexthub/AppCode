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

#include "appcode/fml/platform/android/paths_android.h"

#include "appcode/fml/file.h"

namespace fml {
namespace paths {

std::pair<bool, std::string> GetExecutablePath() {
  return {false, ""};
}

static std::string gCachesPath;

void InitializeAndroidCachesPath(std::string caches_path) {
  gCachesPath = std::move(caches_path);
}

fml::UniqueFD GetCachesDirectory() {
  // If the caches path is not initialized, the FD will be invalid and caching
  // will be disabled throughout the system.
  return OpenDirectory(gCachesPath.c_str(), false, fml::FilePermission::kRead);
}

}  // namespace paths
}  // namespace fml
