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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_GRAPHICS_APPCODEDARWINEXTERNALTEXTUREMETAL_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_GRAPHICS_APPCODEDARWINEXTERNALTEXTUREMETAL_H_

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

#include "appcode/common/graphics/texture.h"
#include "appcode/display_list/image/dl_image.h"
#include "appcode/impeller/display_list/aiks_context.h"
#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterTexture.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImage.h"

@interface FlutterDarwinExternalTextureSkImageWrapper : NSObject

+ (sk_sp<SkImage>)wrapYUVATexture:(nonnull id<MTLTexture>)yTex
                            UVTex:(nonnull id<MTLTexture>)uvTex
                    YUVColorSpace:(SkYUVColorSpace)colorSpace
                        grContext:(nonnull GrDirectContext*)grContext
                            width:(size_t)width
                           height:(size_t)height;

+ (sk_sp<SkImage>)wrapRGBATexture:(nonnull id<MTLTexture>)rgbaTex
                        grContext:(nonnull GrDirectContext*)grContext
                            width:(size_t)width
                           height:(size_t)height;

@end

@interface FlutterDarwinExternalTextureImpellerImageWrapper : NSObject

+ (sk_sp<appcode::DlImage>)wrapYUVATexture:(nonnull id<MTLTexture>)yTex
                                     UVTex:(nonnull id<MTLTexture>)uvTex
                             YUVColorSpace:(impeller::YUVColorSpace)colorSpace
                               aiksContext:(nonnull impeller::AiksContext*)aiksContext;

+ (sk_sp<appcode::DlImage>)wrapRGBATexture:(nonnull id<MTLTexture>)rgbaTex
                               aiksContext:(nonnull impeller::AiksContext*)aiks_context;

@end

@interface FlutterDarwinExternalTextureMetal : NSObject

- (nullable instancetype)initWithTextureCache:(nonnull CVMetalTextureCacheRef)textureCache
                                    textureID:(int64_t)textureID
                                      texture:(nonnull NSObject<FlutterTexture>*)texture
                               enableImpeller:(BOOL)enableImpeller;

- (void)paintContext:(appcode::Texture::PaintContext&)context
              bounds:(const SkRect&)bounds
              freeze:(BOOL)freeze
            sampling:(const appcode::DlImageSampling)sampling;

- (void)onGrContextCreated;

- (void)onGrContextDestroyed;

- (void)markNewFrameAvailable;

- (void)onTextureUnregistered;

@property(nonatomic, readonly) int64_t textureID;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_GRAPHICS_APPCODEDARWINEXTERNALTEXTUREMETAL_H_
