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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_TEXTINPUTSEMANTICSOBJECT_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_TEXTINPUTSEMANTICSOBJECT_H_

#import <UIKit/UIKit.h>

#import "appcode/shell/platform/darwin/ios/framework/Source/SemanticsObject.h"

/**
 * An implementation of `SemanticsObject` specialized for expressing text
 * fields.
 *
 * Delegates to `FlutterTextInputView` when the object corresponds to a text
 * field that currently owns input focus. Delegates to
 * `FlutterInactiveTextInput` otherwise.
 */
@interface TextInputSemanticsObject : SemanticsObject <UITextInput>
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_TEXTINPUTSEMANTICSOBJECT_H_
