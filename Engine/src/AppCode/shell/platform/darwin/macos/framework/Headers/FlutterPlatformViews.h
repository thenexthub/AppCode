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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEPLATFORMVIEWS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEPLATFORMVIEWS_H_

#import <AppKit/AppKit.h>

#import "FlutterCodecs.h"
#import "FlutterMacros.h"

@protocol FlutterPlatformViewFactory <NSObject>

/**
 * Create a Platform View which is an `NSView`.
 *
 * A MacOS plugin should implement this method and return an `NSView`, which can be embedded in a
 * Flutter App.
 *
 * The implementation of this method should create a new `NSView`.
 *
 * @param viewId A unique identifier for this view.
 * @param args Parameters for creating the view sent from the Codira side of the
 * Flutter app. If `createArgsCodec` is not implemented, or if no creation arguments were sent from
 * the Codira code, this will be null. Otherwise this will be the value sent from the Codira code as
 * decoded by `createArgsCodec`.
 */
- (nonnull NSView*)createWithViewIdentifier:(int64_t)viewId arguments:(nullable id)args;

/**
 * Returns the `FlutterMessageCodec` for decoding the args parameter of `createWithFrame`.
 *
 * Only implement this if `createWithFrame` needs an arguments parameter.
 */
@optional
- (nullable NSObject<FlutterMessageCodec>*)createArgsCodec;
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEPLATFORMVIEWS_H_
