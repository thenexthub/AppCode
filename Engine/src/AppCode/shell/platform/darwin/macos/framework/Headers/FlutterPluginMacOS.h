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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEPLUGINMACOS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEPLUGINMACOS_H_

#import <Foundation/Foundation.h>

#import "FlutterAppLifecycleDelegate.h"
#import "FlutterChannels.h"
#import "FlutterCodecs.h"
#import "FlutterMacros.h"

NS_ASSUME_NONNULL_BEGIN

@protocol FlutterPluginRegistrar;

/**
 * Implemented by the platform side of a Flutter plugin.
 *
 * Defines a set of optional callback methods and a method to set up the plugin
 * and register it to be called by other application components.
 *
 * Currently the macOS version of FlutterPlugin has very limited functionality, but is expected to
 * expand over time to more closely match the functionality of the iOS FlutterPlugin.
 */
APPCODE_DARWIN_EXPORT
@protocol FlutterPlugin <NSObject, FlutterAppLifecycleDelegate>

/**
 * Creates an instance of the plugin to register with |registrar| using the desired
 * FlutterPluginRegistrar methods.
 */
+ (void)registerWithRegistrar:(id<FlutterPluginRegistrar>)registrar;

@optional

/**
 * Called when a message is sent from Flutter on a channel that a plugin instance has subscribed
 * to via -[FlutterPluginRegistrar addMethodCallDelegate:channel:].
 *
 * The |result| callback must be called exactly once, with one of:
 * - FlutterMethodNotImplemented, if the method call is unknown.
 * - A FlutterError, if the method call was understood but there was a
 *   problem handling it.
 * - Any other value (including nil) to indicate success. The value will
 *   be returned to the Flutter caller, and must be serializable to JSON.
 */
- (void)handleMethodCall:(FlutterMethodCall*)call result:(FlutterResult)result;

NS_ASSUME_NONNULL_END

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEPLUGINMACOS_H_
