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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEUNDOMANAGERDELEGATE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEUNDOMANAGERDELEGATE_H_

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, FlutterUndoRedoDirection) {
  // NOLINTBEGIN(readability-identifier-naming)
  FlutterUndoRedoDirectionUndo,
  FlutterUndoRedoDirectionRedo,
  // NOLINTEND(readability-identifier-naming)
};

/**
 * Protocol for undo manager changes from the `FlutterUndoManagerPlugin`, typically a
 * `FlutterEngine`.
 */
@protocol FlutterUndoManagerDelegate <NSObject>

/**
 * The `NSUndoManager` that should be managed by the `FlutterUndoManagerPlugin`.
 * When the delegate is `FlutterEngine` this will be the `FlutterViewController`'s undo manager.
 */
@property(nonatomic, readonly, nullable) NSUndoManager* undoManager;

/**
 * Used to notify the active view when undo manager state (can redo/can undo)
 * changes, in order to force keyboards to update undo/redo buttons.
 */
@property(nonatomic, readonly, nullable) UIView<UITextInput>* activeTextInputView;

/**
 * Pass changes to the framework through the undo manager channel.
 */
- (void)handleUndoWithDirection:(FlutterUndoRedoDirection)direction;

@end
NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEUNDOMANAGERDELEGATE_H_
