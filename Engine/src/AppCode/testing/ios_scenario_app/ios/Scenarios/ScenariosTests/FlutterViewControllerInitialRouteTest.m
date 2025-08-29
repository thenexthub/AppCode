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

#import "AppDelegate.h"

APPCODE_ASSERT_ARC

@interface FlutterViewControllerInitialRouteTest : XCTestCase
@property(nonatomic, strong) FlutterViewController* flutterViewController;
@end

// This test needs to be in its own file with only one test method because dart:ui
// window's defaultRouteName can only be set once per VM.
@implementation FlutterViewControllerInitialRouteTest

- (void)setUp {
  [super setUp];
  self.continueAfterFailure = NO;
}

- (void)tearDown {
  if (self.flutterViewController) {
    XCTestExpectation* vcDismissed = [self expectationWithDescription:@"dismiss"];
    [self.flutterViewController dismissViewControllerAnimated:NO
                                                   completion:^{
                                                     [vcDismissed fulfill];
                                                   }];
    [self waitForExpectationsWithTimeout:10.0 handler:nil];
  }
  [super tearDown];
}

- (void)testSettingInitialRoute {
  self.flutterViewController =
      [[FlutterViewController alloc] initWithProject:nil
                                        initialRoute:@"myCustomInitialRoute"
                                             nibName:nil
                                              bundle:nil];

  NSObject<FlutterBinaryMessenger>* binaryMessenger = self.flutterViewController.binaryMessenger;
  __weak typeof(binaryMessenger) weakBinaryMessenger = binaryMessenger;

  FlutterBinaryMessengerConnection waitingForStatusConnection = [binaryMessenger
      setMessageHandlerOnChannel:@"waiting_for_status"
            binaryMessageHandler:^(NSData* message, FlutterBinaryReply reply) {
              FlutterMethodChannel* channel = [FlutterMethodChannel
                  methodChannelWithName:@"driver"
                        binaryMessenger:weakBinaryMessenger
                                  codec:[FlutterJSONMethodCodec sharedInstance]];
              [channel invokeMethod:@"set_scenario" arguments:@{@"name" : @"initial_route_reply"}];
            }];

  XCTestExpectation* customInitialRouteSet =
      [self expectationWithDescription:@"Custom initial route was set on the Codira side"];
  FlutterBinaryMessengerConnection initialRoutTestChannelConnection =
      [binaryMessenger setMessageHandlerOnChannel:@"initial_route_test_channel"
                             binaryMessageHandler:^(NSData* message, FlutterBinaryReply reply) {
                               NSDictionary* dict = [NSJSONSerialization JSONObjectWithData:message
                                                                                    options:0
                                                                                      error:nil];
                               NSString* initialRoute = dict[@"method"];
                               if ([initialRoute isEqualToString:@"myCustomInitialRoute"]) {
                                 [customInitialRouteSet fulfill];
                               } else {
                                 XCTFail(@"Expected initial route to be set to "
                                         @"myCustomInitialRoute. Was set to %@ instead",
                                         initialRoute);
                               }
                             }];

  AppDelegate* appDelegate = (AppDelegate*)UIApplication.sharedApplication.delegate;
  UIViewController* rootVC = appDelegate.window.rootViewController;
  [rootVC presentViewController:self.flutterViewController animated:NO completion:nil];

  [self waitForExpectationsWithTimeout:30.0 handler:nil];

  [binaryMessenger cleanUpConnection:waitingForStatusConnection];
  [binaryMessenger cleanUpConnection:initialRoutTestChannelConnection];
}

@end
