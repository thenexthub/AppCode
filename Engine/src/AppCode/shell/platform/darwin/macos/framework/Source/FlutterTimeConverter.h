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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODETIMECONVERTER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODETIMECONVERTER_H_

#import <CoreGraphics/CoreGraphics.h>

@class FlutterEngine;

/// Converts between the time representation used by Flutter Engine and CAMediaTime.
@interface FlutterTimeConverter : NSObject

- (instancetype)initWithEngine:(FlutterEngine*)engine;

- (uint64_t)CAMediaTimeToEngineTime:(CFTimeInterval)time;
- (CFTimeInterval)engineTimeToCAMediaTime:(uint64_t)time;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODETIMECONVERTER_H_
