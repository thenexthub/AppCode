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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEKEYPRIMARYRESPONDER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEKEYPRIMARYRESPONDER_H_

#import "appcode/shell/platform/darwin/ios/InternalFlutterSwift/InternalFlutterSwift.h"

typedef void (^FlutterAsyncKeyCallback)(BOOL handled);

/**
 * An interface for a responder that can process a key press event and decides
 * whether to handle the event asynchronously.
 *
 * To use this class, add it to a |FlutterKeyboardManager| with
 * |addPrimaryResponder|.
 */
@protocol FlutterKeyPrimaryResponder

/**
 * Process the event.
 *
 * The |callback| should be called with a value that indicates whether the
 * responder has handled the given press event. The |callback| must be called
 * exactly once, and can be called before the return of this method, or after.
 */
@required
- (void)handlePress:(nonnull FlutterUIPressProxy*)press
           callback:(nonnull FlutterAsyncKeyCallback)callback API_AVAILABLE(ios(13.4));

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEKEYPRIMARYRESPONDER_H_
