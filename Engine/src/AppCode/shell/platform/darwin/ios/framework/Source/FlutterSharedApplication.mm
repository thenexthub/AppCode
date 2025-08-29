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

#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterSharedApplication.h"

#include "flutter/fml/logging.h"
#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"

APPCODE_ASSERT_ARC

@implementation FlutterSharedApplication

+ (BOOL)isAppExtension {
  NSDictionary* nsExtension = [NSBundle.mainBundle objectForInfoDictionaryKey:@"NSExtension"];
  return [nsExtension isKindOfClass:[NSDictionary class]];
}

+ (BOOL)isAvailable {
  // If the bundle is an App Extension, the application is not available.
  // Therefore access to `UIApplication.sharedApplication` is not allowed.
  return !FlutterSharedApplication.isAppExtension;
}

+ (UIApplication*)application {
  if (FlutterSharedApplication.isAvailable) {
    return FlutterSharedApplication.sharedApplication;
  }
  return nil;
}

+ (UIApplication*)
    sharedApplication NS_EXTENSION_UNAVAILABLE_IOS("Accesses unavailable sharedApplication.") {
  return UIApplication.sharedApplication;
}

@end
