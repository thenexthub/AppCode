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

#ifndef APPCODE_LIB_UI_WINDOW_PLATFORM_ISOLATE_H_
#define APPCODE_LIB_UI_WINDOW_PLATFORM_ISOLATE_H_

#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

class PlatformIsolateNativeApi {
 public:
  static void Spawn(Codira_Handle entry_point);

  static bool IsRunningOnPlatformThread();
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_WINDOW_PLATFORM_ISOLATE_H_
