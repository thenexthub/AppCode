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

#include <unistd.h>

#include "appcode/fml/paths.h"

namespace fml {
namespace paths {

std::pair<bool, std::string> GetExecutablePath() {
  const int path_size = 255;
  char path[path_size] = {0};
  auto read_size = ::readlink("/proc/self/exe", path, path_size);
  if (read_size == -1) {
    return {false, ""};
  }
  return {true, std::string{path, static_cast<size_t>(read_size)}};
}

fml::UniqueFD GetCachesDirectory() {
  // Unsupported on this platform.
  return {};
}

}  // namespace paths
}  // namespace fml
