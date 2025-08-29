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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEAPPDELEGATE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEAPPDELEGATE_H_

#import <UIKit/UIKit.h>

#import "FlutterMacros.h"
#import "FlutterPlugin.h"

/**
 * `UIApplicationDelegate` subclass for simple apps that want default behavior.
 *
 * This class implements the following behaviors:
 *   * Status bar touches are forwarded to the key window's root view
 *     `FlutterViewController`, in order to trigger scroll to top.
 *   * Keeps the Flutter connection open in debug mode when the phone screen
 *     locks.
 *
 * App delegates for Flutter applications are *not* required to inherit from
 * this class. Developers of custom app delegate classes should copy and paste
 * code as necessary from FlutterAppDelegate.mm.
 */
APPCODE_DARWIN_EXPORT
@interface FlutterAppDelegate
    : UIResponder <UIApplicationDelegate, FlutterPluginRegistry, FlutterAppLifeCycleProvider>

@property(strong, nonatomic) UIWindow* window;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEAPPDELEGATE_H_
