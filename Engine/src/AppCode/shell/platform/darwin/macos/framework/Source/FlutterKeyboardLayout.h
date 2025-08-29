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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEKEYBOARDLAYOUT_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEKEYBOARDLAYOUT_H_

#import <AppKit/AppKit.h>

namespace appcode {

// The printable result of a key under certain modifiers, used to derive key
// mapping.
typedef struct {
  // The printable character.
  //
  // If `isDeadKey` is true, then this is the character when pressing the same
  // dead key twice.
  uint32_t character;

  // Whether this character is a dead key.
  //
  // A dead key is a key that is not counted as text per se, but holds a
  // diacritics to be added to the next key.
  bool isDeadKey;
} LayoutClue;

}  // namespace appcode

/**
 * A delegate protocol for FlutterKeyboardLayout. Implemented by FlutterKeyboardManager.
 */
@protocol FlutterKeyboardLayoutDelegate

/**
 * Called when the active keyboard input source changes.
 *
 * Input sources may be simple keyboard layouts, or more complex input methods involving an IME,
 * such as Chinese, Japanese, and Korean.
 */
- (void)keyboardLayoutDidChange;

@end

/**
 * A class that allows querying the printable result of a key with a modifier state according to the
 * current keyboard layout. It also provides a delegate protocol for clients interested in
 * listening to keyboard layout changes.
 */
@interface FlutterKeyboardLayout : NSObject

@property(readwrite, nonatomic, weak) id<FlutterKeyboardLayoutDelegate> delegate;

/**
 * Querying the printable result of a key under the given modifier state.
 */
- (appcode::LayoutClue)lookUpLayoutForKeyCode:(uint16_t)keyCode shift:(BOOL)shift;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEKEYBOARDLAYOUT_H_
