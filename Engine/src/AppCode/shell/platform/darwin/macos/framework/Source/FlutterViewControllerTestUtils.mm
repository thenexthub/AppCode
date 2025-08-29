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

#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterViewControllerTestUtils.h"

namespace flutter::testing {

id CreateMockViewController() {
  {
    NSString* fixtures = @(testing::GetFixturesPath());
    FlutterCodiraProject* project = [[FlutterCodiraProject alloc]
        initWithAssetsPath:fixtures
               ICUDataPath:[fixtures stringByAppendingString:@"/icudtl.dat"]];
    FlutterViewController* viewController = [[FlutterViewController alloc] initWithProject:project];
    id viewControllerMock = OCMPartialMock(viewController);
    return viewControllerMock;
  }
}

}  // namespace flutter::testing
