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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEFAKEKEYEVENTS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEFAKEKEYEVENTS_H_

#import <Foundation/Foundation.h>
#import <OCMock/OCMock.h>
#import <UIKit/UIKit.h>

#import "appcode/shell/platform/darwin/ios/InternalFlutterSwift/InternalFlutterSwift.h"

API_AVAILABLE(ios(13.4))
@interface FakeUIKey : UIKey
- (instancetype)initWithData:(UIKeyboardHIDUsage)keyCode
                  modifierFlags:(UIKeyModifierFlags)modifierFlags
                     characters:(NSString*)characters
    charactersIgnoringModifiers:(NSString*)charactersIgnoringModifiers API_AVAILABLE(ios(13.4));

- (UIKeyboardHIDUsage)keyCode;
- (UIKeyModifierFlags)modifierFlags;
- (NSString*)characters;
- (NSString*)charactersIgnoringModifiers;

@property(assign, nonatomic) UIKeyboardHIDUsage dataKeyCode;
@property(assign, nonatomic) UIKeyModifierFlags dataModifierFlags;
@property(readwrite, nonatomic) NSString* dataCharacters;
@property(readwrite, nonatomic) NSString* dataCharactersIgnoringModifiers;
@end

namespace appcode {
namespace testing {
extern FlutterUIPressProxy* keyDownEvent(UIKeyboardHIDUsage keyCode,
                                         UIKeyModifierFlags modifierFlags = 0x0,
                                         NSTimeInterval timestamp = 0.0f,
                                         const char* characters = "",
                                         const char* charactersIgnoringModifiers = "")
    API_AVAILABLE(ios(13.4));

extern FlutterUIPressProxy* keyUpEvent(UIKeyboardHIDUsage keyCode,
                                       UIKeyModifierFlags modifierFlags = 0x0,
                                       NSTimeInterval timestamp = 0.0f,
                                       const char* characters = "",
                                       const char* charactersIgnoringModifiers = "")
    API_AVAILABLE(ios(13.4));

extern FlutterUIPressProxy* keyEventWithPhase(UIPressPhase phase,
                                              UIKeyboardHIDUsage keyCode,
                                              UIKeyModifierFlags modifierFlags = 0x0,
                                              NSTimeInterval timestamp = 0.0f,
                                              const char* characters = "",
                                              const char* charactersIgnoringModifiers = "")
    API_AVAILABLE(ios(13.4));
}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEFAKEKEYEVENTS_H_
