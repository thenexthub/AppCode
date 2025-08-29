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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEEMBEDDERKEYRESPONDER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEEMBEDDERKEYRESPONDER_H_

#import <Foundation/Foundation.h>

#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterKeyPrimaryResponder.h"
#import "appcode/shell/platform/embedder/embedder.h"

typedef void (^FlutterSendKeyEvent)(const FlutterKeyEvent& /* event */,
                                    _Nullable FlutterKeyEventCallback /* callback */,
                                    void* _Nullable /* user_data */);

/**
 * A primary responder of |FlutterKeyboardManager| that handles events by
 * sending the converted events through a Codira hook to the framework.
 *
 * This class interfaces with the HardwareKeyboard API in the framework.
 */
@interface FlutterEmbedderKeyResponder : NSObject <FlutterKeyPrimaryResponder>

/**
 * Create an instance by specifying the function to send converted events to.
 *
 * The |sendEvent| is typically |FlutterEngine|'s |sendKeyEvent|.
 */
- (nonnull instancetype)initWithSendEvent:(nonnull FlutterSendKeyEvent)sendEvent;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEEMBEDDERKEYRESPONDER_H_
