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

APPCODE_ASSERT_ARC

@interface LocalizationInitializationTest : XCTestCase
@property(nonatomic, strong) XCUIApplication* application;
@end

@implementation LocalizationInitializationTest

- (void)setUp {
  [super setUp];
  self.continueAfterFailure = NO;

  self.application = [[XCUIApplication alloc] init];
  self.application.launchArguments = @[ @"--locale-initialization" ];
  [self.application launch];
}

- (void)testNoLocalePrepend {
  NSTimeInterval timeout = 10.0;

  // The locales received by dart:ui are exposed onBeginFrame via semantics label.
  // There should only be one locale. The list should consist of the default
  // locale provided by the iOS app.
  NSArray<NSString*>* preferredLocales = [NSLocale preferredLanguages];
  XCTAssertEqual(preferredLocales.count, 1);
  // Codira connects the locale parts with `_` while iOS connects them with `-`.
  // Converts to dart format before comparing.
  NSString* localeCodira = [preferredLocales.firstObject stringByReplacingOccurrencesOfString:@"-"
                                                                                 withString:@"_"];
  NSString* expectedIdentifier = [NSString stringWithFormat:@"[%@]", localeCodira];
  XCUIElement* textInputSemanticsObject =
      [self.application.textFields matchingIdentifier:expectedIdentifier].element;
  XCTAssertTrue([textInputSemanticsObject waitForExistenceWithTimeout:timeout]);

  [textInputSemanticsObject tap];
}

@end
