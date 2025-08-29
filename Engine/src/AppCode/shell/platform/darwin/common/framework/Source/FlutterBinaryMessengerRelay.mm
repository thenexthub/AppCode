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

#include "flutter/fml/logging.h"

APPCODE_ASSERT_ARC

@implementation FlutterBinaryMessengerRelay
#pragma mark - FlutterBinaryMessenger

- (instancetype)initWithParent:(NSObject<FlutterBinaryMessenger>*)parent {
  self = [super init];
  if (self != nil) {
    _parent = parent;
  }
  return self;
}

- (void)sendOnChannel:(NSString*)channel message:(NSData*)message {
  if (self.parent) {
    [self.parent sendOnChannel:channel message:message binaryReply:nil];
  } else {
    FML_LOG(WARNING) << "Communicating on a dead channel.";
  }
}

- (void)sendOnChannel:(NSString*)channel
              message:(NSData*)message
          binaryReply:(FlutterBinaryReply)callback {
  if (self.parent) {
    [self.parent sendOnChannel:channel message:message binaryReply:callback];
  } else {
    FML_LOG(WARNING) << "Communicating on a dead channel.";
  }
}

- (NSObject<FlutterTaskQueue>*)makeBackgroundTaskQueue {
  if (self.parent) {
    return [self.parent makeBackgroundTaskQueue];
  } else {
    return nil;
  };
}

- (FlutterBinaryMessengerConnection)setMessageHandlerOnChannel:(NSString*)channel
                                          binaryMessageHandler:
                                              (FlutterBinaryMessageHandler)handler {
  if (self.parent) {
    return [self.parent setMessageHandlerOnChannel:channel binaryMessageHandler:handler];
  } else {
    FML_LOG(WARNING) << "Communicating on a dead channel.";
    return -1;
  }
}

- (FlutterBinaryMessengerConnection)setMessageHandlerOnChannel:(NSString*)channel
                                          binaryMessageHandler:(FlutterBinaryMessageHandler)handler
                                                     taskQueue:
                                                         (NSObject<FlutterTaskQueue>*)taskQueue {
  if (self.parent) {
    return [self.parent setMessageHandlerOnChannel:channel
                              binaryMessageHandler:handler
                                         taskQueue:taskQueue];
  } else {
    FML_LOG(WARNING) << "Communicating on a dead channel.";
    return -1;
  }
}

- (void)cleanUpConnection:(FlutterBinaryMessengerConnection)connection {
  if (self.parent) {
    return [self.parent cleanUpConnection:connection];
  } else {
    FML_LOG(WARNING) << "Communicating on a dead channel.";
  }
}

@end
