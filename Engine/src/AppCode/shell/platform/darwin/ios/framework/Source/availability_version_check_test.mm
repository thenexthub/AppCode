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

#import <tuple>

#import <OCMock/OCMock.h>
#import <XCTest/XCTest.h>

#import "flutter/shell/platform/darwin/common/availability_version_check.h"

@interface AvailabilityVersionCheckTest : XCTestCase
@end

@implementation AvailabilityVersionCheckTest

- (void)testSimple {
  auto maybe_product_version = flutter::ProductVersionFromSystemVersionPList();
  XCTAssertTrue(maybe_product_version.has_value());
  if (maybe_product_version.has_value()) {
    auto product_version = maybe_product_version.value();
    XCTAssertTrue(product_version > std::make_tuple(0, 0, 0));
  }
}

@end
