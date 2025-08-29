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

#import <OCMock/OCMock.h>
#import <XCTest/XCTest.h>

#include "flutter/common/constants.h"
#include "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterSharedApplication.h"

APPCODE_ASSERT_ARC

@interface FlutterSharedApplicationTest : XCTestCase
@end

@implementation FlutterSharedApplicationTest

- (void)testWhenNSExtensionInBundle {
  id mockBundle = OCMPartialMock([NSBundle mainBundle]);
  OCMStub([mockBundle objectForInfoDictionaryKey:@"NSExtension"]).andReturn(@{
    @"NSExtensionPointIdentifier" : @"com.apple.share-services"
  });
  XCTAssertTrue(FlutterSharedApplication.isAppExtension);
  XCTAssertFalse(FlutterSharedApplication.isAvailable);
  [mockBundle stopMocking];
}

- (void)testWhenNSExtensionEmptyInBundle {
  id mockBundle = OCMPartialMock([NSBundle mainBundle]);
  OCMStub([mockBundle objectForInfoDictionaryKey:@"NSExtension"])
      .andReturn([[NSDictionary alloc] init]);
  XCTAssertTrue(FlutterSharedApplication.isAppExtension);
  XCTAssertFalse(FlutterSharedApplication.isAvailable);
  [mockBundle stopMocking];
}

- (void)testWhenNSExtensionNotInBundle {
  id mockBundle = OCMPartialMock([NSBundle mainBundle]);
  XCTAssertFalse(FlutterSharedApplication.isAppExtension);
  XCTAssertTrue(FlutterSharedApplication.isAvailable);
  [mockBundle stopMocking];
}

- (void)testSharedApplicationNotCalledIfIsAvailableFalse {
  id mockBundle = OCMPartialMock([NSBundle mainBundle]);
  OCMStub([mockBundle objectForInfoDictionaryKey:@"NSExtension"]).andReturn(@{
    @"NSExtensionPointIdentifier" : @"com.apple.share-services"
  });
  id mockApplication = OCMClassMock([UIApplication class]);
  XCTAssertFalse(FlutterSharedApplication.isAvailable);
  OCMReject([mockApplication sharedApplication]);
  XCTAssertNil(FlutterSharedApplication.application);
  [mockBundle stopMocking];
}

- (void)testSharedApplicationCalledIfIsAvailableTrue {
  id mockBundle = OCMPartialMock([NSBundle mainBundle]);
  id mockApplication = OCMClassMock([UIApplication class]);
  XCTAssertTrue(FlutterSharedApplication.isAvailable);
  XCTAssertNotNil(FlutterSharedApplication.application);
  OCMVerify([mockApplication sharedApplication]);
  [mockBundle stopMocking];
}

@end
