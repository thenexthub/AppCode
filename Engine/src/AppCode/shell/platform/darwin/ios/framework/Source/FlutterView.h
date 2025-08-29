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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEVIEW_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEVIEW_H_

#import <Metal/Metal.h>
#import <UIKit/UIKit.h>

#include "appcode/shell/common/rasterizer.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterPlatformViews_Internal.h"

@protocol FlutterViewEngineDelegate <NSObject>

@property(nonatomic, readonly) FlutterPlatformViewsController* platformViewsController;

- (appcode::Rasterizer::Screenshot)takeScreenshot:(appcode::Rasterizer::ScreenshotType)type
                                  asBase64Encoded:(BOOL)base64Encode;

/**
 * A callback that is called when iOS queries accessibility information of the Flutter view.
 *
 * This is useful to predict the current iOS accessibility status. For example, there is
 * no API to listen whether voice control is turned on or off. The Flutter engine uses
 * this callback to enable semantics in order to catch the case that voice control is
 * on.
 */
- (void)appcodeViewAccessibilityDidCall;
@end

@interface FlutterView : UIView

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)initWithFrame:(CGRect)frame NS_UNAVAILABLE;
- (instancetype)initWithCoder:(NSCoder*)aDecoder NS_UNAVAILABLE;

- (instancetype)initWithDelegate:(id<FlutterViewEngineDelegate>)delegate
                          opaque:(BOOL)opaque
                 enableWideGamut:(BOOL)isWideGamutEnabled NS_DESIGNATED_INITIALIZER;

- (UIScreen*)screen;
- (MTLPixelFormat)pixelFormat;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEVIEW_H_
