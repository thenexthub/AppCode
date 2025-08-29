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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODECHANNELKEYRESPONDER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODECHANNELKEYRESPONDER_H_

#import "appcode/shell/platform/darwin/macos/framework/Source/FlutterKeyPrimaryResponder.h"

#import <Cocoa/Cocoa.h>

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterChannels.h"

/**
 * A primary responder of |FlutterKeyboardManager| that handles events by
 * sending the raw information through the method channel.
 *
 * This class communicates with the RawKeyboard API in the framework.
 */
@interface FlutterChannelKeyResponder : NSObject <FlutterKeyPrimaryResponder>

/**
 * Create an instance by specifying the method channel to use.
 */
- (nonnull instancetype)initWithChannel:(nonnull FlutterBasicMessageChannel*)channel;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODECHANNELKEYRESPONDER_H_
