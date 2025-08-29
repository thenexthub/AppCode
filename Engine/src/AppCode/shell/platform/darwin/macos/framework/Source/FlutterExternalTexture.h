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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEEXTERNALTEXTURE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEEXTERNALTEXTURE_H_

#import <Foundation/Foundation.h>

#import "appcode/shell/platform/darwin/graphics/FlutterDarwinContextMetalSkia.h"
#include "appcode/shell/platform/embedder/embedder.h"

/**
 * Embedding side texture wrappers for Metal external textures.
 * Used to bridge FlutterTexture object and handle the texture copy request the
 * Flutter engine.
 */
@interface FlutterExternalTexture : NSObject

/**
 * Initializes a texture adapter with |texture|.
 */
- (nonnull instancetype)initWithFlutterTexture:(nonnull id<FlutterTexture>)texture
                            darwinMetalContext:(nonnull FlutterDarwinContextMetalSkia*)context;

/**
 * Returns the ID for the FlutterExternalTexture instance.
 */
- (int64_t)textureID;

/**
 * Accepts texture buffer copy request from the Flutter engine.
 * When the user side marks the textureID as available, the Flutter engine will
 * callback to this method and ask for populate the |metalTexture| object,
 * such as the texture type and the format of the pixel buffer and the texture object.
 */
- (BOOL)populateTexture:(nonnull FlutterMetalExternalTexture*)metalTexture;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEEXTERNALTEXTURE_H_
