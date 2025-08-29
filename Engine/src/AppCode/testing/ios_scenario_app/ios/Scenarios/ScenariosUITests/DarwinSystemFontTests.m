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

#import <Flutter/Flutter.h>
#import <XCTest/XCTest.h>
#import "GoldenTestManager.h"

@interface DarwinSystemFontTests : XCTestCase

@end

@implementation DarwinSystemFontTests

- (void)testFontRendering {
  self.continueAfterFailure = NO;

  XCUIApplication* application = [[XCUIApplication alloc] init];
  application.launchArguments = @[ @"--darwin-system-font" ];
  [application launch];

  XCUIElement* addTextField = application.textFields[@"ready"];
  XCTAssertTrue([addTextField waitForExistenceWithTimeout:30]);

  GoldenTestManager* manager =
      [[GoldenTestManager alloc] initWithLaunchArg:@"--darwin-system-font"];
  [manager checkGoldenForTest:self rmesThreshold:kDefaultRmseThreshold];
}

@end
