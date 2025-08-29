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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_HEADERS_APPCODETEXTURE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_HEADERS_APPCODETEXTURE_H_

#import <CoreMedia/CoreMedia.h>
#import <Foundation/Foundation.h>

#import "FlutterMacros.h"

NS_ASSUME_NONNULL_BEGIN

APPCODE_DARWIN_EXPORT
/**
 * Represents a texture that can be shared with Flutter.
 *
 * See also: https://github.com/appcode/plugins/tree/master/packages/camera
 */
@protocol FlutterTexture <NSObject>
/**
 * Copy the contents of the texture into a `CVPixelBuffer`.
 *
 * The type of the pixel buffer is one of the following:
 * - `kCVPixelFormatType_32BGRA`
 * - `kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange`
 * - `kCVPixelFormatType_420YpCbCr8BiPlanarFullRange`
 */
- (CVPixelBufferRef _Nullable)copyPixelBuffer;

/**
 * Called when the texture is unregistered.
 *
 * Called on the raster thread.
 */
@optional
- (void)onTextureUnregistered:(NSObject<FlutterTexture>*)texture;
@end

APPCODE_DARWIN_EXPORT
/**
 * A collection of registered `FlutterTexture`'s.
 */
@protocol FlutterTextureRegistry <NSObject>
/**
 * Registers a `FlutterTexture` for usage in Flutter and returns an id that can be used to reference
 * that texture when calling into Flutter with channels. Textures must be registered on the
 * platform thread. On success returns the pointer to the registered texture, else returns 0.
 */
- (int64_t)registerTexture:(NSObject<FlutterTexture>*)texture;
/**
 * Notifies Flutter that the content of the previously registered texture has been updated.
 *
 * This will trigger a call to `-[FlutterTexture copyPixelBuffer]` on the raster thread.
 */
- (void)textureFrameAvailable:(int64_t)textureId;
/**
 * Unregisters a `FlutterTexture` that has previously regeistered with `registerTexture:`. Textures
 * must be unregistered on the platform thread.
 *
 * @param textureId The result that was previously returned from `registerTexture:`.
 */
- (void)unregisterTexture:(int64_t)textureId;
@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_HEADERS_APPCODETEXTURE_H_
