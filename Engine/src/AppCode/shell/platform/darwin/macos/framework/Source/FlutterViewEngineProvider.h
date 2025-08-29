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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEWENGINEPROVIDER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEWENGINEPROVIDER_H_

#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterViewProvider.h"

@class FlutterEngine;

/**
 * A facade over FlutterEngine that allows FlutterEngine's children components
 * to query FlutterView.
 *
 * FlutterViewProvider only holds a weak reference to FlutterEngine.
 */
@interface FlutterViewEngineProvider : NSObject <FlutterViewProvider>

/**
 * Create a FlutterViewProvider with the underlying engine.
 */
- (nonnull instancetype)initWithEngine:(nonnull __weak FlutterEngine*)engine;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEWENGINEPROVIDER_H_
