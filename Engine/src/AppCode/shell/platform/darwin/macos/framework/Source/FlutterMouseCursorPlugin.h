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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEMOUSECURSORPLUGIN_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEMOUSECURSORPLUGIN_H_

#import <Cocoa/Cocoa.h>

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterBinaryMessenger.h"
#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterViewController.h"

@protocol FlutterMouseCursorPluginDelegate <NSObject>
- (void)didUpdateMouseCursor:(nonnull NSCursor*)cursor;
@end

/**
 * A plugin to handle mouse cursor.
 *
 * Responsible for bridging the native macOS mouse cursor system with the
 * Flutter framework mouse cursor classes, via system channels.
 */
@interface FlutterMouseCursorPlugin : NSObject <FlutterPlugin>

+ (void)registerWithRegistrar:(nonnull id<FlutterPluginRegistrar>)registrar
                     delegate:(nullable id<FlutterMouseCursorPluginDelegate>)delegate;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEMOUSECURSORPLUGIN_H_
