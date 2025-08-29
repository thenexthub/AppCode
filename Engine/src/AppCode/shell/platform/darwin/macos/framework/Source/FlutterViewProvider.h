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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEWPROVIDER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEWPROVIDER_H_

#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterView.h"

/**
 * An interface to query FlutterView.
 *
 * See also:
 *
 *  * FlutterViewEngineProvider, a typical implementation.
 */
@protocol FlutterViewProvider

/**
 * Get the FlutterView with the given view ID.
 *
 * Returns nil if the ID is invalid.
 */
- (nullable FlutterView*)viewForIdentifier:(FlutterViewIdentifier)id;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEVIEWPROVIDER_H_
