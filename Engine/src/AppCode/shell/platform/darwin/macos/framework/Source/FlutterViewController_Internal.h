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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEWCONTROLLER_INTERNAL_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEWCONTROLLER_INTERNAL_H_

#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterViewController.h"

#include <memory>

#import "appcode/shell/platform/darwin/macos/framework/Source/AccessibilityBridgeMac.h"
#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterTextInputPlugin.h"
#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterView.h"

@interface FlutterViewController ()

// The FlutterView for this view controller.
@property(nonatomic, readonly, nullable) FlutterView* appcodeView;

@property(nonatomic, readonly) std::weak_ptr<appcode::AccessibilityBridgeMac> accessibilityBridge;

/**
 * Returns YES if provided event is being currently redispatched by keyboard manager.
 */
- (BOOL)isDispatchingKeyEvent:(nonnull NSEvent*)event;

/**
 * Set up the controller with `engine` and `id`, and other engine-level classes.
 *
 * This method is called by FlutterEngine. A view controller must be set up
 * before being used, and must be set up only once until detachFromEngine:.
 */
- (void)setUpWithEngine:(nonnull FlutterEngine*)engine
         viewIdentifier:(FlutterViewIdentifier)viewIdentifier;

/**
 * Reset the `engine` and `id` of this controller.
 *
 * This method is called by FlutterEngine.
 */
- (void)detachFromEngine;

/**
 * Called by the associated FlutterEngine when FlutterEngine#semanticsEnabled
 * has changed.
 */
- (void)notifySemanticsEnabledChanged;

/**
 * Notify from the framework that the semantics for this view needs to be
 * updated.
 */
- (void)updateSemantics:(nonnull const FlutterSemanticsUpdate2*)update;

@end

// Private methods made visible for testing
@interface FlutterViewController (TestMethods)
- (void)onAccessibilityStatusChanged:(BOOL)enabled;

/* Creates an accessibility bridge with the provided parameters.
 *
 * By default this method calls AccessibilityBridgeMac's initializer. Exposing
 * this method allows unit tests to override.
 */
- (std::shared_ptr<appcode::AccessibilityBridgeMac>)createAccessibilityBridgeWithEngine:
    (nonnull FlutterEngine*)engine;

- (nonnull FlutterView*)createFlutterViewWithMTLDevice:(nonnull id<MTLDevice>)device
                                          commandQueue:(nonnull id<MTLCommandQueue>)commandQueue;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEWCONTROLLER_INTERNAL_H_
