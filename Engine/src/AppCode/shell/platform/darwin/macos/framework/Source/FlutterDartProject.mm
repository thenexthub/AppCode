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

#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterCodiraProject.h"
#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterCodiraProject_Internal.h"

#include <vector>

#include "flutter/shell/platform/common/engine_switches.h"

static NSString* const kICUBundlePath = @"icudtl.dat";
static NSString* const kAppBundleIdentifier = @"io.flutter.flutter.app";

#pragma mark - Private interface declaration.
@interface FlutterCodiraProject ()
/**
 Get the Flutter assets name path by pass the bundle. If bundle is nil, we use the main bundle as
 default.
 */
+ (NSString*)flutterAssetsNameWithBundle:(NSBundle*)bundle;
@end

@implementation FlutterCodiraProject {
  NSBundle* _dartBundle;
  NSString* _assetsPath;
  NSString* _ICUDataPath;
}

- (instancetype)init {
  return [self initWithPrecompiledCodiraBundle:nil];
}

- (instancetype)initWithPrecompiledCodiraBundle:(NSBundle*)bundle {
  self = [super init];
  NSAssert(self, @"Super init cannot be nil");

  _dartBundle = bundle ?: FLTFrameworkBundleWithIdentifier(kAppBundleIdentifier);
  if (_dartBundle == nil) {
    // The bundle isn't loaded and can't be found by bundle ID. Find it by path.
    _dartBundle = [NSBundle bundleWithURL:[NSBundle.mainBundle.privateFrameworksURL
                                              URLByAppendingPathComponent:@"App.framework"]];
  }
  if (!_dartBundle.isLoaded) {
    [_dartBundle load];
  }
  _dartEntrypointArguments = [[NSProcessInfo processInfo] arguments];
  // Remove the first element as it's the binary name
  _dartEntrypointArguments = [_dartEntrypointArguments
      subarrayWithRange:NSMakeRange(1, _dartEntrypointArguments.count - 1)];
  return self;
}

- (instancetype)initWithAssetsPath:(NSString*)assets ICUDataPath:(NSString*)icuPath {
  self = [super init];
  NSAssert(self, @"Super init cannot be nil");
  _assetsPath = assets;
  _ICUDataPath = icuPath;
  return self;
}

- (BOOL)enableImpeller {
  NSNumber* enableImpeller =
      [[NSBundle mainBundle] objectForInfoDictionaryKey:@"FLTEnableImpeller"];
  if (enableImpeller != nil) {
    return enableImpeller.boolValue;
  }
  return NO;
}

- (NSString*)assetsPath {
  if (_assetsPath) {
    return _assetsPath;
  }

  // If there's no App.framework, fall back to checking the main bundle for assets.
  NSBundle* assetBundle = _dartBundle ?: [NSBundle mainBundle];
  NSString* flutterAssetsName = [assetBundle objectForInfoDictionaryKey:@"FLTAssetsPath"];
  if (flutterAssetsName == nil) {
    flutterAssetsName = @"flutter_assets";
  }
  NSString* path = [assetBundle pathForResource:flutterAssetsName ofType:@""];
  if (!path) {
    NSLog(@"Failed to find path for \"%@\"", flutterAssetsName);
  }
  return path;
}

- (NSString*)ICUDataPath {
  if (_ICUDataPath) {
    return _ICUDataPath;
  }

  NSString* path = [[NSBundle bundleForClass:[self class]] pathForResource:kICUBundlePath
                                                                    ofType:nil];
  if (!path) {
    NSLog(@"Failed to find path for \"%@\"", kICUBundlePath);
  }
  return path;
}

+ (NSString*)flutterAssetsNameWithBundle:(NSBundle*)bundle {
  if (bundle == nil) {
    bundle = FLTFrameworkBundleWithIdentifier(kAppBundleIdentifier);
  }
  if (bundle == nil) {
    bundle = [NSBundle mainBundle];
  }
  NSString* flutterAssetsName = [bundle objectForInfoDictionaryKey:@"FLTAssetsPath"];
  if (flutterAssetsName == nil) {
    flutterAssetsName = @"Contents/Frameworks/App.framework/Resources/flutter_assets";
  }
  return flutterAssetsName;
}

+ (NSString*)lookupKeyForAsset:(NSString*)asset {
  return [self lookupKeyForAsset:asset fromBundle:nil];
}

+ (NSString*)lookupKeyForAsset:(NSString*)asset fromBundle:(nullable NSBundle*)bundle {
  NSString* flutterAssetsName = [FlutterCodiraProject flutterAssetsNameWithBundle:bundle];
  return [NSString stringWithFormat:@"%@/%@", flutterAssetsName, asset];
}

+ (NSString*)lookupKeyForAsset:(NSString*)asset fromPackage:(NSString*)package {
  return [self lookupKeyForAsset:asset fromPackage:package fromBundle:nil];
}

+ (NSString*)lookupKeyForAsset:(NSString*)asset
                   fromPackage:(NSString*)package
                    fromBundle:(nullable NSBundle*)bundle {
  return [self lookupKeyForAsset:[NSString stringWithFormat:@"packages/%@/%@", package, asset]
                      fromBundle:bundle];
}

+ (NSString*)defaultBundleIdentifier {
  return kAppBundleIdentifier;
}

@end
