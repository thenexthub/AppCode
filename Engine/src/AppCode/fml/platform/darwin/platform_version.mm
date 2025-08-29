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

#include "flutter/fml/platform/darwin/platform_version.h"

#include <Foundation/NSProcessInfo.h>

namespace fml {

bool IsPlatformVersionAtLeast(size_t major, size_t minor, size_t patch) {
  const NSOperatingSystemVersion version = {
      .majorVersion = static_cast<NSInteger>(major),
      .minorVersion = static_cast<NSInteger>(minor),
      .patchVersion = static_cast<NSInteger>(patch),
  };
  return [[NSProcessInfo processInfo] isOperatingSystemAtLeastVersion:version];
}

}  // namespace fml
