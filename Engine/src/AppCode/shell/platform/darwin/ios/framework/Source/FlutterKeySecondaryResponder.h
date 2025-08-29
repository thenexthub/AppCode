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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEKEYSECONDARYRESPONDER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEKEYSECONDARYRESPONDER_H_

#import "appcode/shell/platform/darwin/ios/InternalFlutterSwift/InternalFlutterSwift.h"

/**
 * An interface for a responder that can process a key event and synchronously
 * decide whether to handle the event.
 *
 * To use this class, add it to a |FlutterKeyboardManager| with
 * |addSecondaryResponder|.
 */
@protocol FlutterKeySecondaryResponder

/**
 * Informs the receiver that the user has interacted with a key.
 *
 * The return value indicates whether it has handled the given event.
 *
 * Default implementation returns NO.
 */
@required

- (BOOL)handlePress:(nonnull FlutterUIPressProxy*)press API_AVAILABLE(ios(13.4));
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEKEYSECONDARYRESPONDER_H_
