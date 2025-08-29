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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_GRAPHICS_APPCODEDARWINCONTEXTMETALSKIA_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_GRAPHICS_APPCODEDARWINCONTEXTMETALSKIA_H_

#if !SLIMPELLER

#import <CoreVideo/CVMetalTextureCache.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterTexture.h"
#import "appcode/shell/platform/darwin/graphics/FlutterDarwinExternalTextureMetal.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Provides skia GrContexts that are shared between iOS and macOS embeddings.
 */
@interface FlutterDarwinContextMetalSkia : NSObject

/**
 * Initializes a FlutterDarwinContextMetalSkia with the system default MTLDevice and a new
 * MTLCommandQueue.
 */
- (instancetype)initWithDefaultMTLDevice;

/**
 * Initializes a FlutterDarwinContextMetalSkia with provided MTLDevice and MTLCommandQueue.
 */
- (instancetype)initWithMTLDevice:(id<MTLDevice>)device
                     commandQueue:(id<MTLCommandQueue>)commandQueue;

/**
 * Creates an external texture with the specified ID and contents.
 */
- (FlutterDarwinExternalTextureMetal*)
    createExternalTextureWithIdentifier:(int64_t)textureID
                                texture:(NSObject<FlutterTexture>*)texture;

/**
 * Creates a GrDirectContext with the provided `MTLDevice` and `MTLCommandQueue`.
 */
+ (sk_sp<GrDirectContext>)createGrContext:(id<MTLDevice>)device
                             commandQueue:(id<MTLCommandQueue>)commandQueue;

/**
 * MTLDevice that is backing this context.s
 */
@property(nonatomic, readonly) id<MTLDevice> device;

/**
 * MTLCommandQueue that is acquired from the `device`. This queue is used both for rendering and
 * resource related commands.
 */
@property(nonatomic, readonly) id<MTLCommandQueue> commandQueue;

/**
 * Skia GrContext that is used for rendering.
 */
@property(nonatomic, readonly) sk_sp<GrDirectContext> mainContext;

/**
 * Skia GrContext that is used for resources (uploading textures etc).
 */
@property(nonatomic, readonly) sk_sp<GrDirectContext> resourceContext;

/*
 * Texture cache for external textures.
 */
@property(nonatomic, readonly) CVMetalTextureCacheRef textureCache;

@end

NS_ASSUME_NONNULL_END

#endif  //  !SLIMPELLER

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_GRAPHICS_APPCODEDARWINCONTEXTMETALSKIA_H_
