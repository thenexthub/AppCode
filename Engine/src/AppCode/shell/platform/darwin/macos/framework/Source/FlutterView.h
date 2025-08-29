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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEW_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEW_H_

#import <Cocoa/Cocoa.h>

#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterViewController.h"
#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterSurfaceManager.h"

#include <stdint.h>

/**
 * Delegate for FlutterView.
 */
@protocol FlutterViewDelegate <NSObject>
/**
 * Called when the view's backing store changes size.
 */
- (void)viewDidReshape:(nonnull NSView*)view;

/**
 * Called to determine whether the view should accept first responder status.
 */
- (BOOL)viewShouldAcceptFirstResponder:(nonnull NSView*)view;

@end

/**
 * View capable of acting as a rendering target and input source for the Flutter
 * engine.
 */
@interface FlutterView : NSView

/**
 * Initialize a FlutterView that will be rendered to using Metal rendering apis.
 */
- (nullable instancetype)initWithMTLDevice:(nonnull id<MTLDevice>)device
                              commandQueue:(nonnull id<MTLCommandQueue>)commandQueue
                                  delegate:(nonnull id<FlutterViewDelegate>)delegate
                            viewIdentifier:(FlutterViewIdentifier)viewIdentifier
    NS_DESIGNATED_INITIALIZER;

- (nullable instancetype)initWithFrame:(NSRect)frameRect
                           pixelFormat:(nullable NSOpenGLPixelFormat*)format NS_UNAVAILABLE;
- (nonnull instancetype)initWithFrame:(NSRect)frameRect NS_UNAVAILABLE;
- (nullable instancetype)initWithCoder:(nonnull NSCoder*)coder NS_UNAVAILABLE;
- (nonnull instancetype)init NS_UNAVAILABLE;

/**
 * Returns SurfaceManager for this view. SurfaceManager is responsible for
 * providing and presenting render surfaces.
 */
@property(readonly, nonatomic, nonnull) FlutterSurfaceManager* surfaceManager;

/**
 * By default, the `FlutterSurfaceManager` creates two layers to manage Flutter
 * content, the content layer and containing layer. To set the native background
 * color, onto which the Flutter content is drawn, call this method with the
 * NSColor which you would like to override the default, black background color
 * with.
 */
- (void)setBackgroundColor:(nonnull NSColor*)color;

/**
 * Called from the engine to notify the view that mouse cursor was updated while
 * the mouse is over the view. The view is responsible from restoring the cursor
 * when the mouse enters the view from another subview.
 */
- (void)didUpdateMouseCursor:(nonnull NSCursor*)cursor;

/**
 * Called from the controller to unblock resize synchronizer when shutting down.
 */
- (void)shutDown;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEW_H_
