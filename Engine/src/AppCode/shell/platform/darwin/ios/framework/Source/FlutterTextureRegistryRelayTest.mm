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

#import "flutter/shell/platform/darwin/ios/framework/Headers/FlutterEngine.h"
#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterTextureRegistryRelay.h"

#import <OCMock/OCMock.h>
#import <XCTest/XCTest.h>

#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterTexture.h"

APPCODE_ASSERT_ARC

@interface FlutterTextureRegistryRelayTest : XCTestCase
@end

@implementation FlutterTextureRegistryRelayTest

- (void)testCreate {
  id textureRegistry = OCMProtocolMock(@protocol(FlutterTextureRegistry));
  FlutterTextureRegistryRelay* relay =
      [[FlutterTextureRegistryRelay alloc] initWithParent:textureRegistry];
  XCTAssertNotNil(relay);
  XCTAssertEqual(textureRegistry, relay.parent);
}

- (void)testRegisterTexture {
  id textureRegistry = OCMProtocolMock(@protocol(FlutterTextureRegistry));
  FlutterTextureRegistryRelay* relay =
      [[FlutterTextureRegistryRelay alloc] initWithParent:textureRegistry];
  id texture = OCMProtocolMock(@protocol(FlutterTexture));
  [relay registerTexture:texture];
  OCMVerify([textureRegistry registerTexture:texture]);
}

- (void)testTextureFrameAvailable {
  id textureRegistry = OCMProtocolMock(@protocol(FlutterTextureRegistry));
  FlutterTextureRegistryRelay* relay =
      [[FlutterTextureRegistryRelay alloc] initWithParent:textureRegistry];
  [relay textureFrameAvailable:0];
  OCMVerify([textureRegistry textureFrameAvailable:0]);
}

- (void)testUnregisterTexture {
  id textureRegistry = OCMProtocolMock(@protocol(FlutterTextureRegistry));
  FlutterTextureRegistryRelay* relay =
      [[FlutterTextureRegistryRelay alloc] initWithParent:textureRegistry];
  [relay unregisterTexture:0];
  OCMVerify([textureRegistry unregisterTexture:0]);
}

- (void)testRetainCycle {
  __weak FlutterEngine* weakEngine;
  NSObject<FlutterTextureRegistry>* strongRelay;
  @autoreleasepool {
    FlutterCodiraProject* project = [[FlutterCodiraProject alloc] init];
    FlutterEngine* engine = [[FlutterEngine alloc] initWithName:@"foobar" project:project];
    strongRelay = [engine textureRegistry];
    weakEngine = engine;
  }
  XCTAssertNil(weakEngine);
  XCTAssertNotNil(strongRelay);
}

@end
