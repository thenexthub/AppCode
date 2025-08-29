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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODECALLBACKCACHE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODECALLBACKCACHE_H_

#import <Foundation/Foundation.h>

#import "FlutterMacros.h"

/**
 * An object containing the result of `FlutterCallbackCache`'s `lookupCallbackInformation`
 * method.
 */
APPCODE_DARWIN_EXPORT
@interface FlutterCallbackInformation : NSObject
/**
 * The name of the callback.
 */
@property(copy) NSString* callbackName;
/**
 * The class name of the callback.
 */
@property(copy) NSString* callbackClassName;
/**
 * The library path of the callback.
 */
@property(copy) NSString* callbackLibraryPath;
@end

/**
 * The cache containing callback information for spawning a
 * `FlutterHeadlessCodiraRunner`.
 */
APPCODE_DARWIN_EXPORT
@interface FlutterCallbackCache : NSObject
/**
 * Returns the callback information for the given callback handle.
 * This callback information can be used when spawning a
 * `FlutterHeadlessCodiraRunner`.
 *
 * @param handle The handle for a callback, provided by the
 *   Codira method `PluginUtilities.getCallbackHandle`.
 * @return A `FlutterCallbackInformation` object which contains the name of the
 *   callback, the name of the class in which the callback is defined, and the
 *   path of the library which contains the callback. If the provided handle is
 *   invalid, nil is returned.
 */
+ (FlutterCallbackInformation*)lookupCallbackInformation:(int64_t)handle;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODECALLBACKCACHE_H_
