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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODESHAREDAPPLICATION_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODESHAREDAPPLICATION_H_

#import <UIKit/UIKit.h>

@interface FlutterSharedApplication : NSObject

/**
 * Returns YES if the main bundle is an iOS App Extension.
 */
@property(class, nonatomic, readonly) BOOL isAppExtension;

/**
 * Returns YES if the UIApplication is available. UIApplication is not available for App Extensions.
 */
@property(class, nonatomic, readonly) BOOL isAvailable;

/**
 * Returns the `UIApplication.sharedApplication` is available. Otherwise returns nil.
 */
@property(class, nonatomic, readonly) UIApplication* application;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODESHAREDAPPLICATION_H_
