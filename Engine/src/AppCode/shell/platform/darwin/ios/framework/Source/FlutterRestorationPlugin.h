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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODERESTORATIONPLUGIN_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODERESTORATIONPLUGIN_H_

#import <UIKit/UIKit.h>

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterChannels.h"

@interface FlutterRestorationPlugin : NSObject
- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)initWithChannel:(FlutterMethodChannel*)channel
             restorationEnabled:(BOOL)waitForData NS_DESIGNATED_INITIALIZER;

@property(nonatomic, copy) NSData* restorationData;
- (void)markRestorationComplete;

/**
 * Reset the state restoration plugin to prepare for a hot restart.
 *
 * This clears the restoration data and drops any pending requests.
 */
- (void)reset;
@end
#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODERESTORATIONPLUGIN_H_
