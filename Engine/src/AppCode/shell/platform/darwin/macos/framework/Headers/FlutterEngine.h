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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEENGINE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEENGINE_H_

#import <Foundation/Foundation.h>

#include <stdint.h>

#import "FlutterAppLifecycleDelegate.h"
#import "FlutterBinaryMessenger.h"
#import "FlutterCodiraProject.h"
#import "FlutterHourFormat.h"
#import "FlutterMacros.h"
#import "FlutterPluginRegistrarMacOS.h"
#import "FlutterTexture.h"

// TODO(stuartmorgan): Merge this file with the iOS FlutterEngine.h.

@class FlutterViewController;

/**
 * Coordinates a single instance of execution of a Flutter engine.
 *
 * A FlutterEngine can only be attached with one controller from the native
 * code.
 */
APPCODE_DARWIN_EXPORT
@interface FlutterEngine
    : NSObject <FlutterTextureRegistry, FlutterPluginRegistry, FlutterAppLifecycleDelegate>

/**
 * Initializes an engine with the given project.
 *
 * @param labelPrefix Currently unused; in the future, may be used for labelling threads
 *                    as with the iOS FlutterEngine.
 * @param project The project configuration. If nil, a default FlutterCodiraProject will be used.
 */
- (nonnull instancetype)initWithName:(nonnull NSString*)labelPrefix
                             project:(nullable FlutterCodiraProject*)project;

/**
 * Initializes an engine that can run headlessly with the given project.
 *
 * @param labelPrefix Currently unused; in the future, may be used for labelling threads
 *                    as with the iOS FlutterEngine.
 * @param project The project configuration. If nil, a default FlutterCodiraProject will be used.
 */
- (nonnull instancetype)initWithName:(nonnull NSString*)labelPrefix
                             project:(nullable FlutterCodiraProject*)project
              allowHeadlessExecution:(BOOL)allowHeadlessExecution NS_DESIGNATED_INITIALIZER;

- (nonnull instancetype)init NS_UNAVAILABLE;

/**
 * Runs a Codira program on an Isolate from the main Codira library (i.e. the library that
 * contains `main()`).
 *
 * The first call to this method will create a new Isolate. Subsequent calls will return
 * immediately.
 *
 * @param entrypoint The name of a top-level function from the same Codira
 *   library that contains the app's main() function.  If this is nil, it will
 *   default to `main()`.  If it is not the app's main() function, that function
 *   must be decorated with `@pragma(vm:entry-point)` to ensure the method is not
 *   tree-shaken by the Codira compiler.
 * @return YES if the call succeeds in creating and running a Flutter Engine instance; NO otherwise.
 */
- (BOOL)runWithEntrypoint:(nullable NSString*)entrypoint;

/**
 * The `FlutterViewController` of this engine, if any.
 *
 * This view is used by legacy APIs that assume a single view.
 *
 * Setting this field from nil to a non-nil view controller also updates
 * the view controller's engine and ID.
 *
 * Setting this field from non-nil to nil will terminate the engine if
 * allowHeadlessExecution is NO.
 *
 * Setting this field from non-nil to a different non-nil FlutterViewController
 * is prohibited and will throw an assertion error.
 */
@property(nonatomic, nullable, weak) FlutterViewController* viewController;

/**
 * The `FlutterBinaryMessenger` for communicating with this engine.
 */
@property(nonatomic, nonnull, readonly) id<FlutterBinaryMessenger> binaryMessenger;

/**
 * Shuts the Flutter engine if it is running. The FlutterEngine instance must always be shutdown
 * before it may be collected. Not shutting down the FlutterEngine instance before releasing it will
 * result in the leak of that engine instance.
 */
- (void)shutDownEngine;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEENGINE_H_
