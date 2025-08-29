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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_GRAPHICS_APPCODEDARWINCONTEXTMETALIMPELLER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_GRAPHICS_APPCODEDARWINCONTEXTMETALIMPELLER_H_

#import <CoreVideo/CVMetalTextureCache.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

#include "appcode/fml/concurrent_message_loop.h"
#include "appcode/fml/platform/darwin/cf_utils.h"
#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterTexture.h"
#import "appcode/shell/platform/darwin/graphics/FlutterDarwinExternalTextureMetal.h"
#include "impeller/renderer/backend/metal/context_mtl.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Provides skia GrContexts that are shared between iOS and macOS embeddings.
 */
@interface FlutterDarwinContextMetalImpeller : NSObject

/**
 * Initializes a FlutterDarwinContextMetalImpeller.
 */
- (instancetype)init:(const impeller::Flags&)flags
    gpuDisabledSyncSwitch:
        (const std::shared_ptr<const fml::SyncSwitch>&)is_gpu_disabled_sync_switch;

/**
 * Creates an external texture with the specified ID and contents.
 */
- (FlutterDarwinExternalTextureMetal*)
    createExternalTextureWithIdentifier:(int64_t)textureID
                                texture:(NSObject<FlutterTexture>*)texture;

/**
 * Impeller context.
 */
@property(nonatomic, readonly) std::shared_ptr<impeller::ContextMTL> context;

/*
 * Texture cache for external textures.
 */
@property(nonatomic, readonly) fml::CFRef<CVMetalTextureCacheRef> textureCache;

@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_GRAPHICS_APPCODEDARWINCONTEXTMETALIMPELLER_H_
