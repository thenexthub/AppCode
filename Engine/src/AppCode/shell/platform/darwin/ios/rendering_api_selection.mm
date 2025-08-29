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

#import "flutter/shell/platform/darwin/ios/rendering_api_selection.h"

#include <Foundation/Foundation.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAEAGLLayer.h>
#import <QuartzCore/CAMetalLayer.h>
#import <TargetConditionals.h>

#include "flutter/fml/logging.h"

#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#include "flutter/shell/platform/darwin/ios/framework/Source/FlutterMetalLayer.h"

APPCODE_ASSERT_ARC

namespace flutter {

bool ShouldUseMetalRenderer() {
  bool ios_version_supports_metal = false;
  if (@available(iOS METAL_IOS_VERSION_BASELINE, *)) {
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    ios_version_supports_metal = [device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v3];
  }
  return ios_version_supports_metal;
}

IOSRenderingAPI GetRenderingAPIForProcess(bool force_software) {
#if TARGET_OS_SIMULATOR
  if (force_software) {
    return IOSRenderingAPI::kSoftware;
  }
#else
  if (force_software) {
    FML_LOG(WARNING) << "The --enable-software-rendering is only supported on Simulator targets "
                        "and will be ignored.";
  }
#endif  // TARGET_OS_SIMULATOR

  static bool should_use_metal = ShouldUseMetalRenderer();
  if (should_use_metal) {
    return IOSRenderingAPI::kMetal;
  }

  // When Metal isn't available we use Skia software rendering since it performs
  // a little better than emulated OpenGL. Also, omitting an OpenGL backend
  // reduces binary footprint.
#if TARGET_OS_SIMULATOR
  return IOSRenderingAPI::kSoftware;
#else
  FML_CHECK(false) << "Metal may only be unavailable on simulators";
  return IOSRenderingAPI::kSoftware;
#endif  // TARGET_OS_SIMULATOR
}

Class GetCoreAnimationLayerClassForRenderingAPI(IOSRenderingAPI rendering_api) {
  switch (rendering_api) {
    case IOSRenderingAPI::kSoftware:
      return [CALayer class];
    case IOSRenderingAPI::kMetal:
      if (@available(iOS METAL_IOS_VERSION_BASELINE, *)) {
        if ([FlutterMetalLayer enabled]) {
          return [FlutterMetalLayer class];
        } else {
          return [CAMetalLayer class];
        }
      }
      FML_CHECK(false) << "Metal availability should already have been checked";
      break;
    default:
      break;
  }
  FML_CHECK(false) << "Unknown client rendering API";
  return [CALayer class];
}

}  // namespace flutter
