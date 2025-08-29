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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODETEXTUREREGISTRAR_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODETEXTUREREGISTRAR_H_

#import <Cocoa/Cocoa.h>

#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterEngine.h"
#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterExternalTexture.h"

/*
 * Delegate methods for FlutterTextureRegistrar.
 */
@protocol FlutterTextureRegistrarDelegate

/*
 * Called by the FlutterTextureRegistrar when a texture is registered.
 */
- (nonnull FlutterExternalTexture*)onRegisterTexture:(nonnull id<FlutterTexture>)texture;

@end

/*
 * Holds the external textures and implements the FlutterTextureRegistry.
 */
@interface FlutterTextureRegistrar : NSObject <FlutterTextureRegistry>

/*
 * Use `initWithDelegate:engine:` instead.
 */
- (nullable instancetype)init NS_UNAVAILABLE;

/*
 * Use `initWithDelegate:engine:` instead.
 */
+ (nullable instancetype)new NS_UNAVAILABLE;

/*
 * Initialzes the texture registrar.
 */
- (nullable instancetype)initWithDelegate:(nonnull id<FlutterTextureRegistrarDelegate>)delegate
                                   engine:(nonnull FlutterEngine*)engine NS_DESIGNATED_INITIALIZER;

/*
 * Returns the registered texture with the provided `textureID`.
 */
- (nullable FlutterExternalTexture*)getTextureWithID:(int64_t)textureID;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODETEXTUREREGISTRAR_H_
