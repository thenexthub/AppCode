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

#include "flutter/fml/paths.h"

#include <Foundation/Foundation.h>

#include "flutter/fml/file.h"

namespace fml {
namespace paths {

std::pair<bool, std::string> GetExecutablePath() {
  @autoreleasepool {
    return {true, [NSBundle mainBundle].executablePath.UTF8String};
  }
}

fml::UniqueFD GetCachesDirectory() {
  @autoreleasepool {
    auto items = [[NSFileManager defaultManager] URLsForDirectory:NSCachesDirectory
                                                        inDomains:NSUserDomainMask];
    if (items.count == 0) {
      return {};
    }

    return OpenDirectory(items[0].fileSystemRepresentation, false, FilePermission::kRead);
  }
}

}  // namespace paths
}  // namespace fml
