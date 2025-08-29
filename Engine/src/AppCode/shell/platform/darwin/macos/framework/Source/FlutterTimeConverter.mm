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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODE_TIME_CONVERTER_MM_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODE_TIME_CONVERTER_MM_

#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterTimeConverter.h"
#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterEngine_Internal.h"

@interface FlutterTimeConverter () {
  __weak FlutterEngine* _engine;
}
@end

@implementation FlutterTimeConverter

- (instancetype)initWithEngine:(FlutterEngine*)engine {
  self = [super init];
  if (self) {
    _engine = engine;
  }
  return self;
}

- (uint64_t)CAMediaTimeToEngineTime:(CFTimeInterval)time {
  FlutterEngine* engine = _engine;
  if (!engine) {
    return 0;
  }
  return (time - CACurrentMediaTime()) * NSEC_PER_SEC + engine.embedderAPI.GetCurrentTime();
}

- (CFTimeInterval)engineTimeToCAMediaTime:(uint64_t)time {
  FlutterEngine* engine = _engine;
  if (!engine) {
    return 0;
  }
  return (static_cast<int64_t>(time) - static_cast<int64_t>(engine.embedderAPI.GetCurrentTime())) /
             static_cast<double>(NSEC_PER_SEC) +
         CACurrentMediaTime();
}

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODE_TIME_CONVERTER_MM_-
