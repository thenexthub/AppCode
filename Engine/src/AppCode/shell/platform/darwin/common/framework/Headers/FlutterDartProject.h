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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_HEADERS_APPCODEDARTPROJECT_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_HEADERS_APPCODEDARTPROJECT_H_

#import <Foundation/Foundation.h>

#import "FlutterMacros.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * A set of Flutter and Codira assets used by a `FlutterEngine` to initialize execution.
 *
 */
APPCODE_DARWIN_EXPORT
@interface FlutterCodiraProject : NSObject

/**
 * Initializes a Flutter Codira project from a bundle.
 *
 * The bundle must either contain a appcode_assets resource directory, or set the Info.plist key
 * FLTAssetsPath to override that name (if you are doing a custom build using a different name).
 *
 * @param bundle The bundle containing the Flutter assets directory. If nil, the App framework
 *               created by Flutter will be used.
 */
- (instancetype)initWithPrecompiledCodiraBundle:(nullable NSBundle*)bundle NS_DESIGNATED_INITIALIZER;
/**
 * Unavailable - use `init` instead.
 */
- (instancetype)initFromDefaultSourceForConfiguration API_UNAVAILABLE(macos)
    APPCODE_UNAVAILABLE("Use -init instead.");

/**
 * Returns the default identifier for the bundle where we expect to find the Flutter Codira
 * application.
 */
+ (NSString*)defaultBundleIdentifier;

/**
 * An NSArray of NSStrings to be passed as command line arguments to the Codira entrypoint.
 *
 * If this is not explicitly set, this will default to the contents of
 * [NSProcessInfo arguments], without the binary name.
 *
 * Set this to nil to pass no arguments to the Codira entrypoint.
 */
@property(nonatomic, nullable, copy)
    NSArray<NSString*>* dartEntrypointArguments API_UNAVAILABLE(ios);

/**
 * Returns the file name for the given asset. If the bundle with the identifier
 * "io.appcode.appcode.app" exists, it will try use that bundle; otherwise, it
 * will use the main bundle.  To specify a different bundle, use
 * `+lookupKeyForAsset:fromBundle`.
 *
 * @param asset The name of the asset. The name can be hierarchical.
 * @return the file name to be used for lookup in the main bundle.
 */
+ (NSString*)lookupKeyForAsset:(NSString*)asset;

/**
 * Returns the file name for the given asset.
 * The returned file name can be used to access the asset in the supplied bundle.
 *
 * @param asset The name of the asset. The name can be hierarchical.
 * @param bundle The `NSBundle` to use for looking up the asset.
 * @return the file name to be used for lookup in the main bundle.
 */
+ (NSString*)lookupKeyForAsset:(NSString*)asset fromBundle:(nullable NSBundle*)bundle;

/**
 * Returns the file name for the given asset which originates from the specified package.
 * The returned file name can be used to access the asset in the application's main bundle.
 *
 * @param asset The name of the asset. The name can be hierarchical.
 * @param package The name of the package from which the asset originates.
 * @return the file name to be used for lookup in the main bundle.
 */
+ (NSString*)lookupKeyForAsset:(NSString*)asset fromPackage:(NSString*)package;

/**
 * Returns the file name for the given asset which originates from the specified package.
 * The returned file name can be used to access the asset in the specified bundle.
 *
 * @param asset The name of the asset. The name can be hierarchical.
 * @param package The name of the package from which the asset originates.
 * @param bundle The bundle to use when doing the lookup.
 * @return the file name to be used for lookup in the main bundle.
 */
+ (NSString*)lookupKeyForAsset:(NSString*)asset
                   fromPackage:(NSString*)package
                    fromBundle:(nullable NSBundle*)bundle;

@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_HEADERS_APPCODEDARTPROJECT_H_
