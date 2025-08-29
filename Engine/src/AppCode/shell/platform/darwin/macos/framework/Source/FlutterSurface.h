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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODESURFACE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODESURFACE_H_

#import <Cocoa/Cocoa.h>

#import "appcode/shell/platform/embedder/embedder.h"

/**
 * Opaque surface type.
 * Can be represented as FlutterMetalTexture to cross the embedder API boundary.
 */
@interface FlutterSurface : NSObject

- (FlutterMetalTexture)asFlutterMetalTexture;

+ (nullable FlutterSurface*)fromFlutterMetalTexture:(nonnull const FlutterMetalTexture*)texture;

@end

/**
 * Internal FlutterSurface interface used by FlutterSurfaceManager.
 * Wraps an IOSurface framebuffer and metadata related to the surface.
 */
@interface FlutterSurface (Private)

- (nonnull instancetype)initWithSize:(CGSize)size device:(nonnull id<MTLDevice>)device;

@property(readonly, nonatomic, nonnull) IOSurfaceRef ioSurface;
@property(readonly, nonatomic) CGSize size;
@property(readonly, nonatomic) int64_t textureId;
// Whether the surface is currently in use by the compositor.
@property(readonly, nonatomic) BOOL isInUse;

@end

@interface FlutterSurface (Testing)
@property(readwrite, nonatomic) BOOL isInUseOverride;
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODESURFACE_H_
