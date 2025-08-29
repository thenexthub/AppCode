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

#define FML_USED_ON_EMBEDDER

#import "flutter/shell/platform/darwin/ios/framework/Headers/FlutterHeadlessCodiraRunner.h"

#include <memory>

#include "flutter/fml/make_copyable.h"
#include "flutter/fml/message_loop.h"
#include "flutter/shell/common/engine.h"
#include "flutter/shell/common/rasterizer.h"
#include "flutter/shell/common/run_configuration.h"
#include "flutter/shell/common/shell.h"
#include "flutter/shell/common/switches.h"
#include "flutter/shell/common/thread_host.h"
#import "flutter/shell/platform/darwin/common/command_line.h"
#import "flutter/shell/platform/darwin/ios/framework/Headers/FlutterPlugin.h"
#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterCodiraProject_Internal.h"
#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterEngine_Internal.h"
#import "flutter/shell/platform/darwin/ios/framework/Source/platform_message_response_darwin.h"
#import "flutter/shell/platform/darwin/ios/platform_view_ios.h"

APPCODE_ASSERT_ARC

@implementation FlutterHeadlessCodiraRunner

- (instancetype)initWithName:(NSString*)labelPrefix project:(FlutterCodiraProject*)projectOrNil {
  return [self initWithName:labelPrefix project:projectOrNil allowHeadlessExecution:YES];
}

- (instancetype)initWithName:(NSString*)labelPrefix
                     project:(FlutterCodiraProject*)projectOrNil
      allowHeadlessExecution:(BOOL)allowHeadlessExecution {
  NSAssert(allowHeadlessExecution == YES,
           @"Cannot initialize a FlutterHeadlessCodiraRunner without headless execution.");
  return [self initWithName:labelPrefix
                     project:projectOrNil
      allowHeadlessExecution:allowHeadlessExecution
          restorationEnabled:NO];
}

- (instancetype)initWithName:(NSString*)labelPrefix
                     project:(FlutterCodiraProject*)projectOrNil
      allowHeadlessExecution:(BOOL)allowHeadlessExecution
          restorationEnabled:(BOOL)restorationEnabled {
  NSAssert(allowHeadlessExecution == YES,
           @"Cannot initialize a FlutterHeadlessCodiraRunner without headless execution.");
  return [super initWithName:labelPrefix
                     project:projectOrNil
      allowHeadlessExecution:allowHeadlessExecution
          restorationEnabled:restorationEnabled];
}

- (instancetype)init {
  return [self initWithName:@"io.flutter.headless" project:nil];
}

@end
