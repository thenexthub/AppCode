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

#import "GoldenPlatformViewTests.h"

#include <sys/sysctl.h>
#import "GoldenTestManager.h"

static const NSInteger kSecondsToWaitForPlatformView = 30;

@interface GoldenPlatformViewTests ()

@property(nonatomic, copy) NSString* goldenName;

@property(nonatomic, strong) GoldenTestManager* manager;

@end

@implementation GoldenPlatformViewTests

- (instancetype)initWithManager:(GoldenTestManager*)manager invocation:(NSInvocation*)invocation {
  self = [super initWithInvocation:invocation];
  _manager = manager;
  _rmseThreadhold = kDefaultRmseThreshold;
  return self;
}

- (void)setUp {
  [super setUp];
  self.continueAfterFailure = NO;

  self.application = [[XCUIApplication alloc] init];
  self.application.launchArguments = @[ self.manager.launchArg ];
  [self.application launch];
}

// Note: don't prefix with "test" or GoldenPlatformViewTests will run instead of the subclasses.
- (void)checkPlatformViewGolden {
  XCUIElement* element = self.application.textViews.firstMatch;
  BOOL exists = [element waitForExistenceWithTimeout:kSecondsToWaitForPlatformView];
  if (!exists) {
    XCTFail(@"It took longer than %@ second to find the platform view."
            @"There might be issues with the platform view's construction,"
            @"or with how the scenario is built.",
            @(kSecondsToWaitForPlatformView));
  }

  [self.manager checkGoldenForTest:self rmesThreshold:self.rmseThreadhold];
}
@end
