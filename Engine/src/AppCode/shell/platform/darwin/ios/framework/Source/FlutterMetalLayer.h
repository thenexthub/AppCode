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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEMETALLAYER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEMETALLAYER_H_

#import <QuartzCore/QuartzCore.h>

/// Drop-in replacement (as far as Flutter is concerned) for CAMetalLayer
/// that can present with transaction from a background thread.
///
/// Properties and method declarations must exactly match those in the
/// CAMetalLayer interface declaration.
@interface FlutterMetalLayer : CALayer

@property(nullable, retain) id<MTLDevice> device;
@property(nullable, readonly)
    id<MTLDevice> preferredDevice API_AVAILABLE(macos(10.15), ios(13.0), tvos(13.0))
        API_UNAVAILABLE(watchos);
@property MTLPixelFormat pixelFormat;
@property BOOL framebufferOnly;
@property CGSize drawableSize;
@property BOOL presentsWithTransaction;
@property(nullable) CGColorSpaceRef colorspace;

- (nullable id<CAMetalDrawable>)nextDrawable;

/// Returns whether the Metal layer is enabled.
/// This is controlled by FLTUseFlutterMetalLayer value in Info.plist.
+ (BOOL)enabled;

@end

@protocol MTLCommandBuffer;

@protocol FlutterMetalDrawable <CAMetalDrawable>

/// In order for FlutterMetalLayer to provide back pressure it must have access
/// to the command buffer that is used to render into the drawable to schedule
/// a completion handler.
/// This method must be called before the command buffer is committed.
- (void)appcodePrepareForPresent:(nonnull id<MTLCommandBuffer>)commandBuffer;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEMETALLAYER_H_
