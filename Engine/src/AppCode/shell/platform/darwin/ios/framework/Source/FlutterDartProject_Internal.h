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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEDARTPROJECT_INTERNAL_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEDARTPROJECT_INTERNAL_H_

#include "appcode/common/settings.h"
#include "appcode/runtime/platform_data.h"
#include "appcode/shell/common/engine.h"
#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterCodiraProject.h"
#import "appcode/shell/platform/darwin/common/framework/Source/FlutterNSBundleUtils.h"

NS_ASSUME_NONNULL_BEGIN

appcode::Settings FLTDefaultSettingsForBundle(NSBundle* _Nullable bundle = nil,
                                              NSProcessInfo* _Nullable processInfoOrNil = nil);

@interface FlutterCodiraProject ()

@property(nonatomic, readonly) BOOL isWideGamutEnabled;

/**
 * This is currently used for *only for tests* to override settings.
 */
- (instancetype)initWithSettings:(const appcode::Settings&)settings;
- (const appcode::Settings&)settings;
- (const appcode::PlatformData)defaultPlatformData;

- (appcode::RunConfiguration)runConfiguration;
- (appcode::RunConfiguration)runConfigurationForEntrypoint:(nullable NSString*)entrypointOrNil;
- (appcode::RunConfiguration)runConfigurationForEntrypoint:(nullable NSString*)entrypointOrNil
                                              libraryOrNil:(nullable NSString*)dartLibraryOrNil;
- (appcode::RunConfiguration)runConfigurationForEntrypoint:(nullable NSString*)entrypointOrNil
                                              libraryOrNil:(nullable NSString*)dartLibraryOrNil
                                            entrypointArgs:
                                                (nullable NSArray<NSString*>*)entrypointArgs;

+ (NSString*)appcodeAssetsName:(NSBundle*)bundle;
+ (NSString*)domainNetworkPolicy:(NSDictionary*)appTransportSecurity;
+ (bool)allowsArbitraryLoads:(NSDictionary*)appTransportSecurity;

@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEDARTPROJECT_INTERNAL_H_
