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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEAPPDELEGATE_INTERNAL_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEAPPDELEGATE_INTERNAL_H_

#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterAppDelegate.h"
#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterEngine_Internal.h"

@interface FlutterAppDelegate ()

/**
 * Holds a weak reference to the termination handler owned by the engine.
 * Called by the |FlutterApplication| when termination is requested by the OS.
 */
@property(readwrite, nullable, weak) FlutterEngineTerminationHandler* terminationHandler;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEAPPDELEGATE_INTERNAL_H_
