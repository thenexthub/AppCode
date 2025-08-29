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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_APPCODE_TASK_QUEUE_DISPATCH_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_APPCODE_TASK_QUEUE_DISPATCH_H_

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterBinaryMessenger.h"

/// The private implementation of `FlutterTaskQueue` that has method
/// declarations.
///
/// `FlutterTaskQueue` doesn't have any methods publicly since it is supposed to
/// be an opaque data structure. For Swift integration though `FlutterTaskQueue`
/// is visible publicly with no methods.
@protocol FlutterTaskQueueDispatch <FlutterTaskQueue>
- (void)dispatch:(dispatch_block_t)block;
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_APPCODE_TASK_QUEUE_DISPATCH_H_
