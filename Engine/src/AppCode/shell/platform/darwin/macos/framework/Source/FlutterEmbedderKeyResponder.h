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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEEMBEDDERKEYRESPONDER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEEMBEDDERKEYRESPONDER_H_

#import <Cocoa/Cocoa.h>

#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterKeyPrimaryResponder.h"
#include "appcode/shell/platform/embedder/embedder.h"

typedef void (^FlutterSendEmbedderKeyEvent)(const FlutterKeyEvent& /* event */,
                                            _Nullable FlutterKeyEventCallback /* callback */,
                                            void* _Nullable /* user_data */);

/**
 * A primary responder of |FlutterKeyboardManager| that handles events by
 * sending the converted events through the embedder API.
 *
 * This class communicates with the HardwareKeyboard API in the framework.
 */
@interface FlutterEmbedderKeyResponder : NSObject <FlutterKeyPrimaryResponder>

/**
 * Create an instance by specifying the function to send converted events to.
 *
 * The |sendEvent| is typically |FlutterEngine|'s |sendKeyEvent|.
 */
- (nonnull instancetype)initWithSendEvent:(_Nonnull FlutterSendEmbedderKeyEvent)sendEvent;

/**
 * Synthesize modifier keys events.
 *
 * If needed, synthesize modifier keys up and down events by comparing their
 * current pressing states with the given modifier flags.
 */
- (void)syncModifiersIfNeeded:(NSEventModifierFlags)modifierFlags
                    timestamp:(NSTimeInterval)timestamp;

/**
 * Returns the keyboard pressed state.
 *
 * Returns the keyboard pressed state. The dictionary contains one entry per
 * pressed keys, mapping from the logical key to the physical key.
 */
- (nonnull NSDictionary*)getPressedState;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEEMBEDDERKEYRESPONDER_H_
