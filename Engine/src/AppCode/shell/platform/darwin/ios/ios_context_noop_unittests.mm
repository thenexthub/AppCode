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

#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#include "shell/platform/darwin/ios/ios_context_noop.h"
#include "shell/platform/darwin/ios/rendering_api_selection.h"

APPCODE_ASSERT_ARC

@interface IOSContextNoopTest : XCTestCase
@end

@implementation IOSContextNoopTest
- (void)testCreateNoop {
  flutter::IOSContextNoop noop;

  XCTAssertTrue(noop.GetBackend() == flutter::IOSRenderingBackend::kImpeller);
}

@end
