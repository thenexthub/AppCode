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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODERESIZESYNCHRONIZER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODERESIZESYNCHRONIZER_H_

#import <Cocoa/Cocoa.h>

#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterViewController.h"
#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterRunLoop.h"

/**
 * Class responsible for coordinating window resize with content update.
 */
@interface FlutterResizeSynchronizer : NSObject

/**
 * Begins a resize operation for the given size. Block the thread until
 * performCommitForSize: with the same size is called.
 * While the thread is blocked Flutter messages are being pumped.
 * See [FlutterRunLoop pollFlutterMessagesOnce].
 */
- (void)beginResizeForSize:(CGSize)size notify:(nonnull dispatch_block_t)notify;

/**
 * Called from raster thread. Schedules the given block on platform thread
 * at given delay and unblocks the platform thread if waiting for the surface
 * during resize.
 */
- (void)performCommitForSize:(CGSize)size
                      notify:(nonnull dispatch_block_t)notify
                       delay:(NSTimeInterval)delay;

/**
 * Called when the view is shut down. Unblocks platform thread if blocked
 * during resize.
 */
- (void)shutDown;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODERESIZESYNCHRONIZER_H_
