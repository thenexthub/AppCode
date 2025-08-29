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

#import <XCTest/XCTest.h>

#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#import "flutter/shell/platform/darwin/ios/framework/Source/UIViewController+FlutterScreenAndSceneIfLoaded.h"

APPCODE_ASSERT_ARC

@interface UIViewController_FlutterViewAndSceneIfLoadedTest : XCTestCase
@end

@implementation UIViewController_FlutterViewAndSceneIfLoadedTest

- (void)testWindowSceneIfViewLoadedReturnsWindowSceneIfViewLoaded {
  UIViewController* viewController = [[UIViewController alloc] initWithNibName:nil bundle:nil];

  NSSet<UIScene*>* scenes = UIApplication.sharedApplication.connectedScenes;
  XCTAssertEqual(scenes.count, 1UL, @"There must only be 1 scene for test");
  UIScene* scene = scenes.anyObject;
  XCTAssert([scene isKindOfClass:[UIWindowScene class]], @"Must be a window scene for test");
  UIWindowScene* windowScene = (UIWindowScene*)scene;
  XCTAssert(windowScene.windows.count > 0, @"There must be at least 1 window for test");
  UIWindow* window = windowScene.windows[0];
  [window addSubview:viewController.view];

  [viewController loadView];
  XCTAssertEqual(viewController.flutterWindowSceneIfViewLoaded, windowScene,
                 @"Must return the correct window scene when view loaded");
}

- (void)testWindowSceneIfViewLoadedReturnsNilIfViewNotLoaded {
  UIViewController* viewController = [[UIViewController alloc] initWithNibName:nil bundle:nil];
  XCTAssertNil(viewController.flutterWindowSceneIfViewLoaded,
               @"Must return nil window scene when view not loaded");
}

- (void)testScreenIfViewLoadedReturnsScreenIfViewLoadedAfterIOS13 {
  UIViewController* viewController = [[UIViewController alloc] initWithNibName:nil bundle:nil];

  NSSet<UIScene*>* scenes = UIApplication.sharedApplication.connectedScenes;
  XCTAssertEqual(scenes.count, 1UL, @"There must only be 1 scene for test");
  UIScene* scene = scenes.anyObject;
  XCTAssert([scene isKindOfClass:[UIWindowScene class]], @"Must be a window scene for test");
  UIWindowScene* windowScene = (UIWindowScene*)scene;
  XCTAssert(windowScene.windows.count > 0, @"There must be at least 1 window for test");
  UIWindow* window = windowScene.windows[0];
  [window addSubview:viewController.view];

  [viewController loadView];
  XCTAssertEqual(viewController.flutterScreenIfViewLoaded, windowScene.screen,
                 @"Must return the correct screen when view loaded");
}

- (void)testScreenIfViewLoadedReturnsNilIfViewNotLoadedAfterIOS13 {
  UIViewController* viewController = [[UIViewController alloc] initWithNibName:nil bundle:nil];
  XCTAssertNil(viewController.flutterScreenIfViewLoaded,
               @"Must return nil screen when view not loaded");
}

@end
