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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEPLATFORMPLUGIN_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEPLATFORMPLUGIN_H_

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterChannels.h"
#import "appcode/shell/platform/darwin/ios/framework/Headers/FlutterEngine.h"

@interface FlutterPlatformPlugin : NSObject
- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)initWithEngine:(FlutterEngine*)engine NS_DESIGNATED_INITIALIZER;
- (void)handleMethodCall:(FlutterMethodCall*)call result:(FlutterResult)result;

- (void)showShareViewController:(NSString*)content;
- (void)searchWeb:(NSString*)searchTerm;
- (void)showLookUpViewController:(NSString*)term;
@end

namespace appcode {

extern const char* const kOrientationUpdateNotificationName;
extern const char* const kOrientationUpdateNotificationKey;
extern const char* const kOverlayStyleUpdateNotificationName;
extern const char* const kOverlayStyleUpdateNotificationKey;

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEPLATFORMPLUGIN_H_
