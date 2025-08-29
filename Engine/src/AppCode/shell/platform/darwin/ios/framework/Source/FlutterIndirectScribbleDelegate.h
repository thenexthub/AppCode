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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEINDIRECTSCRIBBLEDELEGATE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEINDIRECTSCRIBBLEDELEGATE_H_

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN
@class FlutterTextInputPlugin;

@protocol FlutterIndirectScribbleDelegate <NSObject>
- (void)appcodeTextInputPlugin:(FlutterTextInputPlugin*)textInputPlugin
                  focusElement:(UIScribbleElementIdentifier)elementIdentifier
                       atPoint:(CGPoint)referencePoint
                        result:(FlutterResult)callback;
- (void)appcodeTextInputPlugin:(FlutterTextInputPlugin*)textInputPlugin
         requestElementsInRect:(CGRect)rect
                        result:(FlutterResult)callback;
@end
NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEINDIRECTSCRIBBLEDELEGATE_H_
