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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEENGINE_TEST_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEENGINE_TEST_H_

#import "appcode/shell/common/shell.h"
#import "appcode/shell/platform/darwin/ios/framework/Headers/FlutterEngine.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterTextInputDelegate.h"
#include "appcode/shell/platform/darwin/ios/platform_view_ios.h"
#import "appcode/shell/platform/darwin/ios/rendering_api_selection.h"
#include "appcode/shell/platform/embedder/embedder.h"

@class FlutterBinaryMessengerRelay;

namespace appcode {
class ThreadHost;
}

// Category to add test-only visibility.
@interface FlutterEngine (Test) <FlutterBinaryMessenger>

@property(readonly, nonatomic) FlutterEngineProcTable& embedderAPI;
@property(readonly, nonatomic) BOOL enableEmbedderAPI;

- (appcode::Shell&)shell;
- (appcode::PlatformViewIOS*)platformView;

- (void)setBinaryMessenger:(FlutterBinaryMessengerRelay*)binaryMessenger;
- (appcode::IOSRenderingAPI)platformViewsRenderingAPI;
- (void)waitForFirstFrame:(NSTimeInterval)timeout callback:(void (^)(BOOL didTimeout))callback;
- (FlutterEngine*)spawnWithEntrypoint:(/*nullable*/ NSString*)entrypoint
                           libraryURI:(/*nullable*/ NSString*)libraryURI
                         initialRoute:(/*nullable*/ NSString*)initialRoute
                       entrypointArgs:(/*nullable*/ NSArray<NSString*>*)entrypointArgs;
- (const appcode::ThreadHost&)threadHost;
- (void)updateDisplays;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
               performAction:(FlutterTextInputAction)action
                  withClient:(int)client;
- (void)sceneWillEnterForeground:(NSNotification*)notification API_AVAILABLE(ios(13.0));
- (void)sceneDidEnterBackground:(NSNotification*)notification API_AVAILABLE(ios(13.0));
- (void)applicationWillEnterForeground:(NSNotification*)notification;
- (void)applicationDidEnterBackground:(NSNotification*)notification;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEENGINE_TEST_H_
