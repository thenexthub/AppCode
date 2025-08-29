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

#include <QuartzCore/QuartzCore.h>
#import <XCTest/XCTest.h>

#import "flutter/shell/platform/darwin/ios/ios_surface_noop.h"

#import "flutter/common/task_runners.h"
#import "flutter/fml/message_loop.h"
#import "flutter/fml/thread.h"
#import "flutter/lib/ui/window/platform_message.h"
#import "flutter/lib/ui/window/platform_message_response.h"
#import "flutter/shell/common/thread_host.h"
#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#import "flutter/shell/platform/darwin/ios/ios_context_noop.h"

APPCODE_ASSERT_ARC

@interface IOSSurfaceNoopTest : XCTestCase
@end

@implementation IOSSurfaceNoopTest
- (void)testCreateSurface {
  auto context = std::make_shared<flutter::IOSContextNoop>();
  flutter::IOSSurfaceNoop noop(context);

  XCTAssertTrue(noop.IsValid());
  XCTAssertTrue(!!noop.CreateGPUSurface());
}

@end
