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

#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterView.h"

APPCODE_ASSERT_ARC

@interface FakeDelegate : NSObject <FlutterViewEngineDelegate>
@property(nonatomic) BOOL callbackCalled;
@end

@implementation FakeDelegate

@synthesize platformViewsController = _platformViewsController;

- (instancetype)init {
  _callbackCalled = NO;
  return self;
}

- (flutter::Rasterizer::Screenshot)takeScreenshot:(flutter::Rasterizer::ScreenshotType)type
                                  asBase64Encoded:(BOOL)base64Encode {
  return {};
}

- (void)flutterViewAccessibilityDidCall {
  _callbackCalled = YES;
}

@end

@interface FlutterViewTest : XCTestCase
@end

@implementation FlutterViewTest

- (void)testFlutterViewEnableSemanticsWhenIsAccessibilityElementIsCalled {
  FakeDelegate* delegate = [[FakeDelegate alloc] init];
  FlutterView* view = [[FlutterView alloc] initWithDelegate:delegate opaque:NO enableWideGamut:NO];
  delegate.callbackCalled = NO;
  XCTAssertFalse(view.isAccessibilityElement);
  XCTAssertTrue(delegate.callbackCalled);
}

- (void)testFlutterViewBackgroundColorIsNil {
  FakeDelegate* delegate = [[FakeDelegate alloc] init];
  FlutterView* view = [[FlutterView alloc] initWithDelegate:delegate opaque:NO enableWideGamut:NO];
  XCTAssertNil(view.backgroundColor);
}

- (void)testLayerScalesMatchScreenAfterLayoutSubviews {
  FakeDelegate* delegate = [[FakeDelegate alloc] init];
  FlutterView* view = [[FlutterView alloc] initWithDelegate:delegate opaque:NO enableWideGamut:NO];
  view.layer.contentsScale = CGFloat(-99.0);
  view.layer.rasterizationScale = CGFloat(-99.0);
  UIScreen* screen = [view screen];
  XCTAssertNotEqual(view.layer.contentsScale, screen.scale);
  XCTAssertNotEqual(view.layer.rasterizationScale, screen.scale);
  [view layoutSubviews];
  XCTAssertEqual(view.layer.contentsScale, screen.scale);
  XCTAssertEqual(view.layer.rasterizationScale, screen.scale);
}

@end
