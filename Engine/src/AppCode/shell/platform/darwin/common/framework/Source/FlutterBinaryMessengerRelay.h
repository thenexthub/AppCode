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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_SOURCE_APPCODEBINARYMESSENGERRELAY_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_SOURCE_APPCODEBINARYMESSENGERRELAY_H_

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterBinaryMessenger.h"
#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"

#if APPCODE_RUNTIME_MODE == APPCODE_RUNTIME_MODE_DEBUG
APPCODE_DARWIN_EXPORT
#endif
@interface FlutterBinaryMessengerRelay : NSObject <FlutterBinaryMessenger>
@property(nonatomic, weak) NSObject<FlutterBinaryMessenger>* parent;
- (instancetype)initWithParent:(NSObject<FlutterBinaryMessenger>*)parent;
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_SOURCE_APPCODEBINARYMESSENGERRELAY_H_
