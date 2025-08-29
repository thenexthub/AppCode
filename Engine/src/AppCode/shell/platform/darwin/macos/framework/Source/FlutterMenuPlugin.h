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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEMENUPLUGIN_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEMENUPLUGIN_H_

#import <AppKit/AppKit.h>

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterBinaryMessenger.h"
#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterPluginMacOS.h"
#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterViewController.h"

/**
 * A plugin to configure and control the native system menu.
 *
 * Responsible for bridging the native macOS menu system with the Flutter
 * framework's PlatformMenuBar class, via method channels.
 */
@interface FlutterMenuPlugin : NSObject <FlutterPlugin>

/**
 * Registers a FlutterMenuPlugin with the given registrar.
 */
+ (void)registerWithRegistrar:(nonnull id<FlutterPluginRegistrar>)registrar;
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEMENUPLUGIN_H_
