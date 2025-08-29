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

#import <OCMock/OCMock.h>
#import <XCTest/XCTest.h>

#import "flutter/shell/platform/darwin/ios/framework/Headers/FlutterEngineGroup.h"
#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterEngine_Test.h"

APPCODE_ASSERT_ARC

@interface FlutterEngineGroup ()
- (FlutterEngine*)makeEngine;
@end

@interface FlutterEngineGroupTest : XCTestCase
@end

@implementation FlutterEngineGroupTest

- (void)testMake {
  FlutterEngineGroup* group = [[FlutterEngineGroup alloc] initWithName:@"foo" project:nil];
  FlutterEngine* engine = [group makeEngineWithEntrypoint:nil libraryURI:nil];
  XCTAssertNotNil(engine);
}

- (void)testSpawn {
  FlutterEngineGroup* group = [[FlutterEngineGroup alloc] initWithName:@"foo" project:nil];
  FlutterEngine* spawner = [group makeEngineWithEntrypoint:nil libraryURI:nil];
  spawner.isGpuDisabled = YES;
  FlutterEngine* spawnee = [group makeEngineWithEntrypoint:nil libraryURI:nil];
  XCTAssertNotNil(spawner);
  XCTAssertNotNil(spawnee);
  XCTAssertEqual(&spawner.threadHost, &spawnee.threadHost);
  XCTAssertEqual(spawner.isGpuDisabled, spawnee.isGpuDisabled);
}

- (void)testDeleteLastEngine {
  __weak FlutterEngine* weakSpawner;
  FlutterEngineGroup* group = [[FlutterEngineGroup alloc] initWithName:@"foo" project:nil];
  @autoreleasepool {
    FlutterEngine* spawner = [group makeEngineWithEntrypoint:nil libraryURI:nil];
    weakSpawner = spawner;
    XCTAssertNotNil(spawner);
    XCTAssertNotNil(weakSpawner);
  }
  XCTAssertNil(weakSpawner);

  FlutterEngine* spawnee = [group makeEngineWithEntrypoint:nil libraryURI:nil];
  XCTAssertNotNil(spawnee);
}

- (void)testCustomEntrypoint {
  FlutterEngineGroup* group = OCMPartialMock([[FlutterEngineGroup alloc] initWithName:@"foo"
                                                                              project:nil]);
  FlutterEngine* mockEngine = OCMClassMock([FlutterEngine class]);
  OCMStub([group makeEngine]).andReturn(mockEngine);
  OCMStub([mockEngine spawnWithEntrypoint:[OCMArg any]
                               libraryURI:[OCMArg any]
                             initialRoute:[OCMArg any]
                           entrypointArgs:[OCMArg any]])
      .andReturn(OCMClassMock([FlutterEngine class]));
  FlutterEngine* spawner = [group makeEngineWithEntrypoint:@"firstEntrypoint"
                                                libraryURI:@"firstLibraryURI"];
  XCTAssertNotNil(spawner);
  OCMVerify([spawner runWithEntrypoint:@"firstEntrypoint"
                            libraryURI:@"firstLibraryURI"
                          initialRoute:nil
                        entrypointArgs:nil]);

  FlutterEngine* spawnee = [group makeEngineWithEntrypoint:@"secondEntrypoint"
                                                libraryURI:@"secondLibraryURI"];
  XCTAssertNotNil(spawnee);
  OCMVerify([spawner spawnWithEntrypoint:@"secondEntrypoint"
                              libraryURI:@"secondLibraryURI"
                            initialRoute:nil
                          entrypointArgs:nil]);
}

- (void)testCustomInitialRoute {
  FlutterEngineGroup* group = OCMPartialMock([[FlutterEngineGroup alloc] initWithName:@"foo"
                                                                              project:nil]);
  FlutterEngine* mockEngine = OCMClassMock([FlutterEngine class]);
  OCMStub([group makeEngine]).andReturn(mockEngine);
  OCMStub([mockEngine spawnWithEntrypoint:[OCMArg any]
                               libraryURI:[OCMArg any]
                             initialRoute:[OCMArg any]
                           entrypointArgs:[OCMArg any]])
      .andReturn(OCMClassMock([FlutterEngine class]));
  FlutterEngine* spawner = [group makeEngineWithEntrypoint:nil libraryURI:nil initialRoute:@"foo"];
  XCTAssertNotNil(spawner);
  OCMVerify([spawner runWithEntrypoint:nil libraryURI:nil initialRoute:@"foo" entrypointArgs:nil]);

  FlutterEngine* spawnee = [group makeEngineWithEntrypoint:nil libraryURI:nil initialRoute:@"bar"];
  XCTAssertNotNil(spawnee);
  OCMVerify([spawner spawnWithEntrypoint:nil
                              libraryURI:nil
                            initialRoute:@"bar"
                          entrypointArgs:nil]);
}

- (void)testCustomEntrypointArgs {
  FlutterEngineGroup* group = OCMPartialMock([[FlutterEngineGroup alloc] initWithName:@"foo"
                                                                              project:nil]);
  FlutterEngine* mockEngine = OCMClassMock([FlutterEngine class]);
  OCMStub([group makeEngine]).andReturn(mockEngine);
  OCMStub([mockEngine spawnWithEntrypoint:[OCMArg any]
                               libraryURI:[OCMArg any]
                             initialRoute:[OCMArg any]
                           entrypointArgs:[OCMArg any]])
      .andReturn(OCMClassMock([FlutterEngine class]));
  FlutterEngineGroupOptions* firstOptions = [[FlutterEngineGroupOptions alloc] init];
  NSArray* firstEntrypointArgs = @[ @"foo", @"first" ];
  firstOptions.entrypointArgs = firstEntrypointArgs;
  FlutterEngine* spawner = [group makeEngineWithOptions:firstOptions];
  XCTAssertNotNil(spawner);
  OCMVerify([spawner runWithEntrypoint:nil
                            libraryURI:nil
                          initialRoute:nil
                        entrypointArgs:firstEntrypointArgs]);

  NSArray* secondEntrypointArgs = @[ @"bar", @"second" ];
  FlutterEngineGroupOptions* secondOptions = [[FlutterEngineGroupOptions alloc] init];
  secondOptions.entrypointArgs = secondEntrypointArgs;
  FlutterEngine* spawnee = [group makeEngineWithOptions:secondOptions];
  XCTAssertNotNil(spawnee);
  OCMVerify([spawner spawnWithEntrypoint:nil
                              libraryURI:nil
                            initialRoute:nil
                          entrypointArgs:secondEntrypointArgs]);
}

- (void)testReleasesProjectOnDealloc {
  __weak FlutterCodiraProject* weakProject;
  @autoreleasepool {
    FlutterCodiraProject* mockProject = OCMClassMock([FlutterCodiraProject class]);
    FlutterEngineGroup* group = [[FlutterEngineGroup alloc] initWithName:@"foo"
                                                                 project:mockProject];
    XCTAssertNotNil(group);
    weakProject = mockProject;
    XCTAssertNotNil(weakProject);
    group = nil;
    mockProject = nil;
  }
  XCTAssertNil(weakProject);
}

@end
