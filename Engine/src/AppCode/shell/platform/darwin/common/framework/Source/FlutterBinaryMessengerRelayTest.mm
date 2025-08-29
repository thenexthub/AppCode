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

#import "flutter/shell/platform/darwin/common/framework/Source/FlutterBinaryMessengerRelay.h"

#import <OCMock/OCMock.h>

#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#import "flutter/shell/platform/darwin/common/framework/Source/FlutterTestUtils.h"
#import "flutter/shell/platform/darwin/ios/flutter_task_queue_dispatch.h"
#import "flutter/testing/testing.h"
#include "gtest/gtest.h"

APPCODE_ASSERT_ARC

@interface FlutterBinaryMessengerRelayTest : NSObject
@end

@implementation FlutterBinaryMessengerRelayTest

- (void)testCreate {
  id messenger = OCMProtocolMock(@protocol(FlutterBinaryMessenger));
  FlutterBinaryMessengerRelay* relay =
      [[FlutterBinaryMessengerRelay alloc] initWithParent:messenger];
  EXPECT_NE(relay, nil);
  EXPECT_EQ(messenger, relay.parent);
}

- (void)testPassesCallOn {
  id messenger = OCMProtocolMock(@protocol(FlutterBinaryMessenger));
  FlutterBinaryMessengerRelay* relay =
      [[FlutterBinaryMessengerRelay alloc] initWithParent:messenger];
  char messageData[] = {'a', 'a', 'r', 'o', 'n'};
  NSData* message = [NSData dataWithBytes:messageData length:sizeof(messageData)];
  NSString* channel = @"foobar";
  [relay sendOnChannel:channel message:message binaryReply:nil];
  OCMVerify([messenger sendOnChannel:channel message:message binaryReply:nil]);
}

- (void)testDoesntPassCallOn {
  id messenger = OCMStrictProtocolMock(@protocol(FlutterBinaryMessenger));
  FlutterBinaryMessengerRelay* relay =
      [[FlutterBinaryMessengerRelay alloc] initWithParent:messenger];
  char messageData[] = {'a', 'a', 'r', 'o', 'n'};
  NSData* message = [NSData dataWithBytes:messageData length:sizeof(messageData)];
  NSString* channel = @"foobar";
  relay.parent = nil;
  [relay sendOnChannel:channel message:message binaryReply:nil];
}

- (void)testSetMessageHandlerWithTaskQueue {
  id messenger = OCMProtocolMock(@protocol(FlutterBinaryMessenger));
  FlutterBinaryMessengerRelay* relay =
      [[FlutterBinaryMessengerRelay alloc] initWithParent:messenger];
  NSString* channel = @"foobar";
  NSObject<FlutterTaskQueue>* taskQueue = OCMProtocolMock(@protocol(FlutterTaskQueueDispatch));
  FlutterBinaryMessageHandler handler = ^(NSData* _Nullable, FlutterBinaryReply _Nonnull) {
  };
  [relay setMessageHandlerOnChannel:channel binaryMessageHandler:handler taskQueue:taskQueue];
  OCMVerify([messenger setMessageHandlerOnChannel:channel
                             binaryMessageHandler:handler
                                        taskQueue:taskQueue]);
}

- (void)testMakeBackgroundTaskQueue {
  id messenger = OCMProtocolMock(@protocol(FlutterBinaryMessenger));
  FlutterBinaryMessengerRelay* relay =
      [[FlutterBinaryMessengerRelay alloc] initWithParent:messenger];
  [relay makeBackgroundTaskQueue];
  OCMVerify([messenger makeBackgroundTaskQueue]);
}

@end

TEST(FlutterBinaryMessengerRelayTest, Create) {
  ASSERT_FALSE(FLTThrowsObjcException(^{
    [[FlutterBinaryMessengerRelayTest alloc] testCreate];
  }));
}

TEST(FlutterBinaryMessengerRelayTest, PassesCallOn) {
  ASSERT_FALSE(FLTThrowsObjcException(^{
    [[FlutterBinaryMessengerRelayTest alloc] testPassesCallOn];
  }));
}

TEST(FlutterBinaryMessengerRelayTest, DoesntPassCallOn) {
  ASSERT_FALSE(FLTThrowsObjcException(^{
    [[FlutterBinaryMessengerRelayTest alloc] testDoesntPassCallOn];
  }));
}

TEST(FlutterBinaryMessengerRelayTest, SetMessageHandlerWithTaskQueue) {
  ASSERT_FALSE(FLTThrowsObjcException(^{
    [[FlutterBinaryMessengerRelayTest alloc] testSetMessageHandlerWithTaskQueue];
  }));
}

TEST(FlutterBinaryMessengerRelayTest, SetMakeBackgroundTaskQueue) {
  ASSERT_FALSE(FLTThrowsObjcException(^{
    [[FlutterBinaryMessengerRelayTest alloc] testMakeBackgroundTaskQueue];
  }));
}
