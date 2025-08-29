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

#include <Carbon/Carbon.h>
#import <Foundation/Foundation.h>
#import <OCMock/OCMock.h>

#include "flutter/common/constants.h"
#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterEngineTestUtils.h"
#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterEngine_Internal.h"
#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterViewController_Internal.h"
#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterViewEngineProvider.h"
#import "flutter/testing/testing.h"
#include "third_party/googletest/googletest/include/gtest/gtest.h"

namespace flutter::testing {

TEST(FlutterViewEngineProviderUnittests, GetViewReturnsTheCorrectView) {
  FlutterViewEngineProvider* viewProvider;
  id mockEngine = CreateMockFlutterEngine(@"");
  __block id mockFlutterViewController;
  OCMStub([mockEngine viewControllerForIdentifier:0])
      .ignoringNonObjectArgs()
      .andDo(^(NSInvocation* invocation) {
        FlutterViewIdentifier viewIdentifier;
        [invocation getArgument:&viewIdentifier atIndex:2];
        if (viewIdentifier == kFlutterImplicitViewId) {
          if (mockFlutterViewController != nil) {
            [invocation setReturnValue:&mockFlutterViewController];
          }
        }
      });
  viewProvider = [[FlutterViewEngineProvider alloc] initWithEngine:mockEngine];

  // When the view controller is not set, the returned view is nil.
  EXPECT_EQ([viewProvider viewForIdentifier:0], nil);

  // When the view controller is set, the returned view is the controller's view.
  mockFlutterViewController = OCMStrictClassMock([FlutterViewController class]);
  id mockView = OCMStrictClassMock([FlutterView class]);
  OCMStub([mockFlutterViewController flutterView]).andReturn(mockView);
  EXPECT_EQ([viewProvider viewForIdentifier:0], mockView);
}

}  // namespace flutter::testing
