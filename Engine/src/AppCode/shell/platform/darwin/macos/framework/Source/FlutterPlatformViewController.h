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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEPLATFORMVIEWCONTROLLER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEPLATFORMVIEWCONTROLLER_H_

#import <Cocoa/Cocoa.h>

#import "FlutterChannels.h"

#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterPlatformViews.h"

#include <map>
#include <unordered_set>

@interface FlutterPlatformViewController : NSViewController
@end

@interface FlutterPlatformViewController ()

/**
 * Creates a platform view of viewType with viewId and arguments passed from
 * the framework's creationParams constructor parameter.
 * FlutterResult is updated to contain nil for success or to contain
 * a FlutterError if there is an error.
 */
- (void)onCreateWithViewIdentifier:(int64_t)viewId
                          viewType:(nonnull NSString*)viewType
                         arguments:(nullable id)args
                            result:(nonnull FlutterResult)result;

/**
 * Disposes the platform view with `viewId`.
 * FlutterResult is updated to contain nil for success or a FlutterError if there is an error.
 */
- (void)onDisposeWithViewID:(int64_t)viewId result:(nonnull FlutterResult)result;

/**
 * Returns the platform view associated with the viewId.
 */
- (nullable NSView*)platformViewWithID:(int64_t)viewId;

/**
 * Register a view factory by adding an entry into the platformViewFactories map with key factoryId
 * and value factory.
 */
- (void)registerViewFactory:(nonnull NSObject<FlutterPlatformViewFactory>*)factory
                     withId:(nonnull NSString*)factoryId;

/**
 * Handles platform view related method calls, for example create, dispose, etc.
 */
- (void)handleMethodCall:(nonnull FlutterMethodCall*)call result:(nonnull FlutterResult)result;

/**
 * Removes platform views slated to be disposed via method handler calls.
 */
- (void)disposePlatformViews;

/**
 * Removes all platform views.
 */
- (void)reset;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEPLATFORMVIEWCONTROLLER_H_
