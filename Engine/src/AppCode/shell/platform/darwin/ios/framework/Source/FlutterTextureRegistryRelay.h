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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODETEXTUREREGISTRYRELAY_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODETEXTUREREGISTRYRELAY_H_

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterTexture.h"

#if APPCODE_RUNTIME_MODE == APPCODE_RUNTIME_MODE_DEBUG
APPCODE_DARWIN_EXPORT
#endif

/**
 * Wrapper around a weakly held collection of registered textures.
 *
 * Avoids a retain cycle between plugins and the engine.
 */
@interface FlutterTextureRegistryRelay : NSObject <FlutterTextureRegistry>

/**
 * A weak reference to a FlutterEngine that will be passed texture registration.
 */
@property(nonatomic, weak) NSObject<FlutterTextureRegistry>* parent;
- (instancetype)initWithParent:(NSObject<FlutterTextureRegistry>*)parent;
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODETEXTUREREGISTRYRELAY_H_
