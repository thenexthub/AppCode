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

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

#import "flutter/shell/platform/darwin/macos/framework/Source/TestFlutterPlatformView.h"

@implementation TestFlutterPlatformView

- (instancetype)initWithFrame:(CGRect)frame arguments:(nullable NSDictionary*)args {
  self = [super initWithFrame:frame];
  _args = args;
  return self;
}

@end

@implementation TestFlutterPlatformViewFactory
- (NSView*)createWithViewIdentifier:(int64_t)viewId arguments:(nullable id)args {
  return [[TestFlutterPlatformView alloc] initWithFrame:CGRectZero arguments:args];
}

- (NSObject<FlutterMessageCodec>*)createArgsCodec {
  return [FlutterStandardMessageCodec sharedInstance];
}

@end
