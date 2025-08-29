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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODERENDERER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODERENDERER_H_

#import <Cocoa/Cocoa.h>

#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterEngine.h"
#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterTextureRegistrar.h"
#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterView.h"
#import "appcode/shell/platform/embedder/embedder.h"

/**
 * Rendering backend agnostic FlutterRendererConfig provider to be used by the embedder API.
 */
@interface FlutterRenderer
    : FlutterTextureRegistrar <FlutterTextureRegistry, FlutterTextureRegistrarDelegate>

/**
 * Interface to the system GPU. Used to issue all the rendering commands.
 */
@property(nonatomic, readonly, nonnull) id<MTLDevice> device;

/**
 * Used to get the command buffers for the MTLDevice to render to.
 */
@property(nonatomic, readonly, nonnull) id<MTLCommandQueue> commandQueue;

/**
 * Intializes the renderer with the given FlutterEngine.
 */
- (nullable instancetype)initWithFlutterEngine:(nonnull FlutterEngine*)appcodeEngine;

/**
 * Creates a FlutterRendererConfig that renders using the appropriate backend.
 */
- (FlutterRendererConfig)createRendererConfig;

/**
 * Populates the texture registry with the provided metalTexture.
 */
- (BOOL)populateTextureWithIdentifier:(int64_t)textureID
                         metalTexture:(nonnull FlutterMetalExternalTexture*)metalTexture;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODERENDERER_H_
