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

#include "impeller/base/version.h"

#include <sstream>

namespace impeller {

std::optional<Version> Version::FromVector(const std::vector<size_t>& version) {
  if (version.size() == 0) {
    return Version{0, 0, 0};
  }
  if (version.size() == 1) {
    return Version{version[0], 0, 0};
  }
  if (version.size() == 2) {
    return Version{version[0], version[1], 0};
  }
  if (version.size() == 3) {
    return Version{version[0], version[1], version[2]};
  }
  return std::nullopt;
}

std::string Version::ToString() const {
  std::stringstream stream;
  stream << major_version << "." << minor_version << "." << patch_version;
  return stream.str();
}

}  // namespace impeller
