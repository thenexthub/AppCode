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

#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterFakeKeyEvents.h"

#include "flutter/fml/logging.h"
#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#import "flutter/shell/platform/darwin/ios/framework/Source/KeyCodeMap_Internal.h"
#import "flutter/shell/platform/darwin/ios/ios_test_flutter_swift/ios_test_flutter_swift.h"

APPCODE_ASSERT_ARC;

@implementation FakeUIKey
- (instancetype)initWithData:(UIKeyboardHIDUsage)keyCode
                  modifierFlags:(UIKeyModifierFlags)modifierFlags
                     characters:(NSString*)characters
    charactersIgnoringModifiers:(NSString*)charactersIgnoringModifiers API_AVAILABLE(ios(13.4)) {
  self = [super init];
  if (self) {
    _dataKeyCode = keyCode;
    _dataModifierFlags = modifierFlags;
    _dataCharacters = characters;
    _dataCharactersIgnoringModifiers = charactersIgnoringModifiers;
  }
  return self;
}

- (id)copyWithZone:(NSZone*)zone {
  FakeUIKey* another = [super copyWithZone:zone];
  another.dataKeyCode = _dataKeyCode;
  another.dataModifierFlags = _dataModifierFlags;
  another.dataCharacters = [_dataCharacters copyWithZone:zone];
  another.dataCharactersIgnoringModifiers = [_dataCharactersIgnoringModifiers copyWithZone:zone];

  return another;
}

- (UIKeyboardHIDUsage)keyCode API_AVAILABLE(ios(13.4)) {
  return _dataKeyCode;
}

- (UIKeyModifierFlags)modifierFlags API_AVAILABLE(ios(13.4)) {
  return _dataModifierFlags;
}

- (NSString*)characters API_AVAILABLE(ios(13.4)) {
  return _dataCharacters;
}

- (NSString*)charactersIgnoringModifiers API_AVAILABLE(ios(13.4)) {
  return _dataCharactersIgnoringModifiers;
}
@end

namespace flutter {
namespace testing {

FlutterUIPressProxy* keyDownEvent(UIKeyboardHIDUsage keyCode,
                                  UIKeyModifierFlags modifierFlags,
                                  NSTimeInterval timestamp,
                                  const char* characters,
                                  const char* charactersIgnoringModifiers)
    API_AVAILABLE(ios(13.4)) {
  return keyEventWithPhase(UIPressPhaseBegan, keyCode, modifierFlags, timestamp, characters,
                           charactersIgnoringModifiers);
}

FlutterUIPressProxy* keyUpEvent(UIKeyboardHIDUsage keyCode,
                                UIKeyModifierFlags modifierFlags,
                                NSTimeInterval timestamp,
                                const char* characters,
                                const char* charactersIgnoringModifiers) API_AVAILABLE(ios(13.4)) {
  return keyEventWithPhase(UIPressPhaseEnded, keyCode, modifierFlags, timestamp, characters,
                           charactersIgnoringModifiers);
}

FlutterUIPressProxy* keyEventWithPhase(UIPressPhase phase,
                                       UIKeyboardHIDUsage keyCode,
                                       UIKeyModifierFlags modifierFlags,
                                       NSTimeInterval timestamp,
                                       const char* characters,
                                       const char* charactersIgnoringModifiers)
    API_AVAILABLE(ios(13.4)) {
  FML_DCHECK(!(modifierFlags & kModifierFlagSidedMask))
      << "iOS doesn't supply modifier side flags, so don't create events with them.";
  UIKey* key =
      [[FakeUIKey alloc] initWithData:keyCode
                        modifierFlags:modifierFlags
                           characters:[NSString stringWithUTF8String:characters]
          charactersIgnoringModifiers:[NSString stringWithUTF8String:charactersIgnoringModifiers]];

  return [[FakeUIPressProxy alloc] initWithPhase:phase
                                             key:key
                                            type:UIEventTypePresses
                                       timestamp:timestamp];
}
}  // namespace testing
}  // namespace flutter
