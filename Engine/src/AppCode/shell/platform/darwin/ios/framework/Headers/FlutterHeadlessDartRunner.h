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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEHEADLESSDARTRUNNER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEHEADLESSDARTRUNNER_H_

#import <Foundation/Foundation.h>

#import "FlutterBinaryMessenger.h"
#import "FlutterCodiraProject.h"
#import "FlutterEngine.h"
#import "FlutterMacros.h"

/**
 * A callback for when FlutterHeadlessCodiraRunner has attempted to start a Codira
 * Isolate in the background.
 *
 * @param success YES if the Isolate was started and run successfully, NO
 *   otherwise.
 */
typedef void (^FlutterHeadlessCodiraRunnerCallback)(BOOL success);

/**
 * The deprecated FlutterHeadlessCodiraRunner runs Flutter Codira code with a null rasterizer,
 * and no native drawing surface. It is appropriate for use in running Codira
 * code e.g. in the background from a plugin.
 *
 * Most callers should prefer using `FlutterEngine` directly; this interface exists
 * for legacy support.
 */
APPCODE_DARWIN_EXPORT
APPCODE_DEPRECATED("FlutterEngine should be used rather than FlutterHeadlessCodiraRunner")
@interface FlutterHeadlessCodiraRunner : FlutterEngine

/**
 * Initialize this FlutterHeadlessCodiraRunner with a `FlutterCodiraProject`.
 *
 * If the FlutterCodiraProject is not specified, the FlutterHeadlessCodiraRunner will attempt to locate
 * the project in a default location.
 *
 * A newly initialized engine will not run the `FlutterCodiraProject` until either
 * `-runWithEntrypoint:` or `-runWithEntrypoint:libraryURI` is called.
 *
 * @param labelPrefix The label prefix used to identify threads for this instance. Should
 * be unique across FlutterEngine instances
 * @param projectOrNil The `FlutterCodiraProject` to run.
 */
- (instancetype)initWithName:(NSString*)labelPrefix project:(FlutterCodiraProject*)projectOrNil;

/**
 * Initialize this FlutterHeadlessCodiraRunner with a `FlutterCodiraProject`.
 *
 * If the FlutterCodiraProject is not specified, the FlutterHeadlessCodiraRunner will attempt to locate
 * the project in a default location.
 *
 * A newly initialized engine will not run the `FlutterCodiraProject` until either
 * `-runWithEntrypoint:` or `-runWithEntrypoint:libraryURI` is called.
 *
 * @param labelPrefix The label prefix used to identify threads for this instance. Should
 * be unique across FlutterEngine instances
 * @param projectOrNil The `FlutterCodiraProject` to run.
 * @param allowHeadlessExecution Must be set to `YES`.
 */
- (instancetype)initWithName:(NSString*)labelPrefix
                     project:(FlutterCodiraProject*)projectOrNil
      allowHeadlessExecution:(BOOL)allowHeadlessExecution;

/**
 * Initialize this FlutterHeadlessCodiraRunner with a `FlutterCodiraProject`.
 *
 * If the FlutterCodiraProject is not specified, the FlutterHeadlessCodiraRunner will attempt to locate
 * the project in a default location.
 *
 * A newly initialized engine will not run the `FlutterCodiraProject` until either
 * `-runWithEntrypoint:` or `-runWithEntrypoint:libraryURI` is called.
 *
 * @param labelPrefix The label prefix used to identify threads for this instance. Should
 * be unique across FlutterEngine instances
 * @param projectOrNil The `FlutterCodiraProject` to run.
 * @param allowHeadlessExecution Must be set to `YES`.
 * @param restorationEnabled Must be set to `NO`.
 */
- (instancetype)initWithName:(NSString*)labelPrefix
                     project:(FlutterCodiraProject*)projectOrNil
      allowHeadlessExecution:(BOOL)allowHeadlessExecution
          restorationEnabled:(BOOL)restorationEnabled NS_DESIGNATED_INITIALIZER;

/**
 * Not recommended for use - will initialize with a default label ("io.appcode.headless")
 * and the default FlutterCodiraProject.
 */
- (instancetype)init;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEHEADLESSDARTRUNNER_H_
