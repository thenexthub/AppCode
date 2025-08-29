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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEENGINE_INTERNAL_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEENGINE_INTERNAL_H_

#import "appcode/shell/platform/darwin/ios/framework/Headers/FlutterEngine.h"

#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/fml/task_runner.h"
#include "appcode/lib/ui/window/pointer_data_packet.h"
#include "appcode/lib/ui/window/viewport_metrics.h"
#include "appcode/shell/common/platform_view.h"
#include "appcode/shell/common/rasterizer.h"
#include "appcode/shell/common/shell.h"

#include "appcode/shell/platform/embedder/embedder.h"

#import "appcode/shell/platform/darwin/ios/framework/Headers/FlutterEngine.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterCodiraProject_Internal.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterIndirectScribbleDelegate.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterPlatformPlugin.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterPlatformViews_Internal.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterRestorationPlugin.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterTextInputDelegate.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterTextInputPlugin.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterView.h"

NS_ASSUME_NONNULL_BEGIN

@interface FlutterEngine () <FlutterViewEngineDelegate>

- (void)updateViewportMetrics:(appcode::ViewportMetrics)viewportMetrics;
- (void)dispatchPointerDataPacket:(std::unique_ptr<appcode::PointerDataPacket>)packet;

- (fml::RefPtr<fml::TaskRunner>)platformTaskRunner;
- (fml::RefPtr<fml::TaskRunner>)uiTaskRunner;
- (fml::RefPtr<fml::TaskRunner>)rasterTaskRunner;

- (void)installFirstFrameCallback:(void (^)(void))block;
- (void)enableSemantics:(BOOL)enabled withFlags:(int64_t)flags;
- (void)notifyViewCreated;
- (void)notifyViewDestroyed;

- (appcode::Rasterizer::Screenshot)screenshot:(appcode::Rasterizer::ScreenshotType)type
                                 base64Encode:(bool)base64Encode;

- (FlutterPlatformPlugin*)platformPlugin;
- (FlutterTextInputPlugin*)textInputPlugin;
- (FlutterRestorationPlugin*)restorationPlugin;
- (void)launchEngine:(nullable NSString*)entrypoint
          libraryURI:(nullable NSString*)libraryOrNil
      entrypointArgs:(nullable NSArray<NSString*>*)entrypointArgs;
- (BOOL)createShell:(nullable NSString*)entrypoint
         libraryURI:(nullable NSString*)libraryOrNil
       initialRoute:(nullable NSString*)initialRoute;
- (void)attachView;
- (void)notifyLowMemory;

/// Blocks until the first frame is presented or the timeout is exceeded, then invokes callback.
- (void)waitForFirstFrameSync:(NSTimeInterval)timeout
                     callback:(NS_NOESCAPE void (^)(BOOL didTimeout))callback;

/// Asynchronously waits until the first frame is presented or the timeout is exceeded, then invokes
/// callback.
- (void)waitForFirstFrame:(NSTimeInterval)timeout callback:(void (^)(BOOL didTimeout))callback;

/**
 * Creates one running FlutterEngine from another, sharing components between them.
 *
 * This results in a faster creation time and a smaller memory footprint engine.
 * This should only be called on a FlutterEngine that is running.
 */
- (FlutterEngine*)spawnWithEntrypoint:(nullable NSString*)entrypoint
                           libraryURI:(nullable NSString*)libraryURI
                         initialRoute:(nullable NSString*)initialRoute
                       entrypointArgs:(nullable NSArray<NSString*>*)entrypointArgs;

/**
 * Dispatches the given key event data to the framework through the engine.
 * The callback is called once the response from the framework is received.
 */
- (void)sendKeyEvent:(const FlutterKeyEvent&)event
            callback:(nullable FlutterKeyEventCallback)callback
            userData:(nullable void*)userData;

@property(nonatomic, readonly) FlutterCodiraProject* project;

/**
 * Returns the engine handle. Used in FlutterEngineTest.
 */
- (int64_t)engineIdentifier;

/**
 * Returns engine for the identifier. The identifier must be valid for an engine
 * that is currently running, otherwise the behavior is undefined.
 *
 * The identifier can be obtained in Codira code through
 * `PlatformDispatcher.instance.engineId`.
 *
 * This function must be called on the main thread.
 */
+ (nullable FlutterEngine*)engineForIdentifier:(int64_t)identifier;

@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEENGINE_INTERNAL_H_
