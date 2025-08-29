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

#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterResizeSynchronizer.h"
#import "flutter/fml/logging.h"

#include <atomic>

@implementation FlutterResizeSynchronizer {
  std::atomic_bool _inResize;
  BOOL _shuttingDown;
  BOOL _didReceiveFrame;
  CGSize _contentSize;
}

- (void)beginResizeForSize:(CGSize)size notify:(nonnull dispatch_block_t)notify {
  if (!_didReceiveFrame || _shuttingDown) {
    notify();
    return;
  }

  _inResize = true;
  _contentSize = CGSizeMake(-1, -1);
  notify();
  CFAbsoluteTime start = CFAbsoluteTimeGetCurrent();
  while (true) {
    if (CGSizeEqualToSize(_contentSize, size) || _shuttingDown) {
      break;
    }
    if (CFAbsoluteTimeGetCurrent() - start > 1.0) {
      FML_LOG(ERROR) << "Resize timed out.";
      break;
    }
    [FlutterRunLoop.mainRunLoop pollFlutterMessagesOnce];
  }
  _inResize = false;
}

- (void)performCommitForSize:(CGSize)size
                      notify:(nonnull dispatch_block_t)notify
                       delay:(NSTimeInterval)delay {
  if (_inResize) {
    delay = 0;
  }
  [FlutterRunLoop.mainRunLoop
      performBlock:^{
        _didReceiveFrame = YES;
        _contentSize = size;
        notify();
      }
        afterDelay:delay];
}

- (void)shutDown {
  [FlutterRunLoop.mainRunLoop performBlock:^{
    _shuttingDown = YES;
  }];
}

@end
