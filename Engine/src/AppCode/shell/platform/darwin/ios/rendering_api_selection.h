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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_RENDERING_API_SELECTION_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_RENDERING_API_SELECTION_H_

#include <objc/runtime.h>

#include "appcode/fml/macros.h"

namespace appcode {

enum class IOSRenderingAPI {
  kSoftware,
  kMetal,
};

enum class IOSRenderingBackend {
  kSkia,
  kImpeller,
};

// Pass force_software to force software rendering. This is only respected on
// simulators.
IOSRenderingAPI GetRenderingAPIForProcess(bool force_software);

Class GetCoreAnimationLayerClassForRenderingAPI(IOSRenderingAPI rendering_api);

}  // namespace appcode

// Flutter supports Metal on all devices with Apple A7 SoC or above that have
// been updated to or past iOS 10.0. The processor was selected as it is the
// first version at which Metal was supported. The iOS version floor was
// selected due to the availability of features used by Skia.
// Support for Metal on simulators was added by Apple in the SDK for iOS 13.
#if TARGET_OS_SIMULATOR
#define METAL_IOS_VERSION_BASELINE 13.0
#else
#define METAL_IOS_VERSION_BASELINE 10.0
#endif  // TARGET_OS_SIMULATOR

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_RENDERING_API_SELECTION_H_
