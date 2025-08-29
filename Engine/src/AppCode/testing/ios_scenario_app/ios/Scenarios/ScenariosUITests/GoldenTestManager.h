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

#ifndef APPCODE_TESTING_IOS_SCENARIO_APP_IOS_SCENARIOS_SCENARIOSUITESTS_GOLDENTESTMANAGER_H_
#define APPCODE_TESTING_IOS_SCENARIO_APP_IOS_SCENARIOS_SCENARIOSUITESTS_GOLDENTESTMANAGER_H_

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>
#import "GoldenImage.h"

NS_ASSUME_NONNULL_BEGIN

extern NSDictionary* launchArgsMap;
const extern double kDefaultRmseThreshold;

// Manages a `GoldenPlatformViewTests`.
//
// It creates the correct `identifer` based on the `launchArg`.
// It also generates the correct GoldenImage based on the `identifier`.
@interface GoldenTestManager : NSObject

@property(readonly, strong, nonatomic) GoldenImage* goldenImage;
@property(readonly, copy, nonatomic) NSString* identifier;
@property(readonly, copy, nonatomic) NSString* launchArg;

// Initilize with launchArg.
//
// Crahes if the launchArg is not mapped in `Appdelegate.launchArgsMap`.
- (instancetype)initWithLaunchArg:(NSString*)launchArg;

// Take a sceenshot of the test app and check it has the same pixels with
// goldenImage inside the `GoldenTestManager`.
- (void)checkGoldenForTest:(XCTestCase*)test rmesThreshold:(double)rmesThreshold;

@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_TESTING_IOS_SCENARIO_APP_IOS_SCENARIOS_SCENARIOSUITESTS_GOLDENTESTMANAGER_H_
