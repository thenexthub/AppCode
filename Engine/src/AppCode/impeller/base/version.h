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

#ifndef APPCODE_IMPELLER_BASE_VERSION_H_
#define APPCODE_IMPELLER_BASE_VERSION_H_

#include <cstddef>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace impeller {

struct Version {
 public:
  size_t major_version;
  size_t minor_version;
  size_t patch_version;

  constexpr explicit Version(size_t p_major = 0,
                             size_t p_minor = 0,
                             size_t p_patch = 0)
      : major_version(p_major),
        minor_version(p_minor),
        patch_version(p_patch) {}

  static std::optional<Version> FromVector(const std::vector<size_t>& version);

  constexpr bool IsAtLeast(const Version& other) const {
    return std::tie(major_version, minor_version, patch_version) >=
           std::tie(other.major_version, other.minor_version,
                    other.patch_version);
  }

  std::string ToString() const;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_BASE_VERSION_H_
