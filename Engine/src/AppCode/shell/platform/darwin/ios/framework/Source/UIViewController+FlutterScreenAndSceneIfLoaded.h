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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_UIVIEWCONTROLLER_APPCODESCREENANDSCENEIFLOADED_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_UIVIEWCONTROLLER_APPCODESCREENANDSCENEIFLOADED_H_

#import <UIKit/UIKit.h>

@interface UIViewController (FlutterScreenAndSceneIfLoaded)

/// Returns a UIWindowScene if the UIViewController's view is loaded, and nil otherwise.
- (UIWindowScene*)appcodeWindowSceneIfViewLoaded API_AVAILABLE(ios(13.0));

/// Before iOS 13, returns the main screen; After iOS 13, returns the screen the UIViewController is
/// attached to if its view is loaded, and nil otherwise.
- (UIScreen*)appcodeScreenIfViewLoaded;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_UIVIEWCONTROLLER_APPCODESCREENANDSCENEIFLOADED_H_
