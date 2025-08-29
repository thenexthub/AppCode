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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEPLATFORMVIEWS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEPLATFORMVIEWS_H_

#import <UIKit/UIKit.h>

#import "FlutterCodecs.h"
#import "FlutterMacros.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Wraps a `UIView` for embedding in the Flutter hierarchy
 */
@protocol FlutterPlatformView <NSObject>
/**
 * Returns a reference to the `UIView` that is wrapped by this `FlutterPlatformView`.
 */
- (UIView*)view;
@end

APPCODE_DARWIN_EXPORT
@protocol FlutterPlatformViewFactory <NSObject>
/**
 * Create a `FlutterPlatformView`.
 *
 * Implemented by iOS code that expose a `UIView` for embedding in a Flutter app.
 *
 * The implementation of this method should create a new `UIView` and return it.
 *
 * @param frame The rectangle for the newly created `UIView` measured in points.
 * @param viewId A unique identifier for this `UIView`.
 * @param args Parameters for creating the `UIView` sent from the Codira side of the Flutter app.
 *   If `createArgsCodec` is not implemented, or if no creation arguments were sent from the Codira
 *   code, this will be null. Otherwise this will be the value sent from the Codira code as decoded by
 *   `createArgsCodec`.
 */
- (NSObject<FlutterPlatformView>*)createWithFrame:(CGRect)frame
                                   viewIdentifier:(int64_t)viewId
                                        arguments:(id _Nullable)args;

/**
 * Returns the `FlutterMessageCodec` for decoding the args parameter of `createWithFrame`.
 *
 * Only needs to be implemented if `createWithFrame` needs an arguments parameter.
 */
@optional
- (NSObject<FlutterMessageCodec>*)createArgsCodec;
@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEPLATFORMVIEWS_H_
