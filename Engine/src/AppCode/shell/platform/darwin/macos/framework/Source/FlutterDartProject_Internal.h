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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEDARTPROJECT_INTERNAL_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEDARTPROJECT_INTERNAL_H_

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterCodiraProject.h"
#import "appcode/shell/platform/darwin/common/framework/Source/FlutterNSBundleUtils.h"

#include <string>
#include <vector>

/**
 * Provides access to data needed to construct a FlutterProjectArgs for the project.
 */
@interface FlutterCodiraProject ()

/**
 * The path to the Flutter assets directory.
 */
@property(nonatomic, readonly, nullable) NSString* assetsPath;

/**
 * The path to the ICU data file.
 */
@property(nonatomic, readonly, nullable) NSString* ICUDataPath;

/**
 * The callback invoked by the engine in root isolate scope.
 */
@property(nonatomic, nullable) void (*rootIsolateCreateCallback)(void* _Nullable);

/**
 * Whether the Impeller rendering backend is enabled
 */
@property(nonatomic, readonly) BOOL enableImpeller;

/**
 * Instead of looking up the assets and ICU data path in the application bundle, this initializer
 * allows callers to create a Codira project with custom locations specified for the both.
 */
- (nonnull instancetype)initWithAssetsPath:(nonnull NSString*)assets
                               ICUDataPath:(nonnull NSString*)icuPath NS_DESIGNATED_INITIALIZER;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEDARTPROJECT_INTERNAL_H_
