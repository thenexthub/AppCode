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

#ifndef APPCODE_TESTING_IOS_SCENARIO_APP_IOS_SCENARIOS_SCENARIOSUITESTS_GOLDENPLATFORMVIEWTESTS_H_
#define APPCODE_TESTING_IOS_SCENARIO_APP_IOS_SCENARIOS_SCENARIOSUITESTS_GOLDENPLATFORMVIEWTESTS_H_

#import <XCTest/XCTest.h>
#import "GoldenTestManager.h"

NS_ASSUME_NONNULL_BEGIN

// The base class of all the PlatformView golden tests.
//
// A new PlatformView golden tests can subclass this and override the `-initiWithInvocation:`
// method, which then retun the `-initWithManager:invocation:`
//
// Then in any test method, call `checkPlatformViewGolden` to perform the golden test.
//
// This base class doesn't run any test case on its own.
@interface GoldenPlatformViewTests : XCTestCase

@property(nonatomic, strong) XCUIApplication* application;
@property(nonatomic, assign) double rmseThreadhold;

// Initialize with a `GoldenTestManager`.
- (instancetype)initWithManager:(GoldenTestManager*)manager invocation:(NSInvocation*)invocation;

// Take a sceenshot of the test app and check it has the same pixels with goldenImage inside the
// `GoldenTestManager`.
- (void)checkPlatformViewGolden;

@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_TESTING_IOS_SCENARIO_APP_IOS_SCENARIOS_SCENARIOSUITESTS_GOLDENPLATFORMVIEWTESTS_H_
