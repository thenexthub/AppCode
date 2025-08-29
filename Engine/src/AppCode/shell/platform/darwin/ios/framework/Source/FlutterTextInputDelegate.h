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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODETEXTINPUTDELEGATE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODETEXTINPUTDELEGATE_H_

#import <Foundation/Foundation.h>

@class FlutterTextInputPlugin;
@class FlutterTextInputView;

typedef NS_ENUM(NSInteger, FlutterTextInputAction) {
  // NOLINTBEGIN(readability-identifier-naming)
  FlutterTextInputActionUnspecified,
  FlutterTextInputActionDone,
  FlutterTextInputActionGo,
  FlutterTextInputActionSend,
  FlutterTextInputActionSearch,
  FlutterTextInputActionNext,
  FlutterTextInputActionContinue,
  FlutterTextInputActionJoin,
  FlutterTextInputActionRoute,
  FlutterTextInputActionEmergencyCall,
  FlutterTextInputActionNewline,
  // NOLINTEND(readability-identifier-naming)
};

typedef NS_ENUM(NSInteger, FlutterFloatingCursorDragState) {
  // NOLINTBEGIN(readability-identifier-naming)
  FlutterFloatingCursorDragStateStart,
  FlutterFloatingCursorDragStateUpdate,
  FlutterFloatingCursorDragStateEnd,
  // NOLINTEND(readability-identifier-naming)
};

@protocol FlutterTextInputDelegate <NSObject>
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
         updateEditingClient:(int)client
                   withState:(NSDictionary*)state;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
         updateEditingClient:(int)client
                   withState:(NSDictionary*)state
                     withTag:(NSString*)tag;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
         updateEditingClient:(int)client
                   withDelta:(NSDictionary*)state;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
               performAction:(FlutterTextInputAction)action
                  withClient:(int)client;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
        updateFloatingCursor:(FlutterFloatingCursorDragState)state
                  withClient:(int)client
                withPosition:(NSDictionary*)point;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
    showAutocorrectionPromptRectForStart:(NSUInteger)start
                                     end:(NSUInteger)end
                              withClient:(int)client;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView showToolbar:(int)client;
- (void)appcodeTextInputViewScribbleInteractionBegan:(FlutterTextInputView*)textInputView;
- (void)appcodeTextInputViewScribbleInteractionFinished:(FlutterTextInputView*)textInputView;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
    insertTextPlaceholderWithSize:(CGSize)size
                       withClient:(int)client;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView removeTextPlaceholder:(int)client;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
    didResignFirstResponderWithTextInputClient:(int)client;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
    willDismissEditMenuWithTextInputClient:(int)client;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
           shareSelectedText:(NSString*)selectedText;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
    searchWebWithSelectedText:(NSString*)selectedText;
- (void)appcodeTextInputView:(FlutterTextInputView*)textInputView
          lookUpSelectedText:(NSString*)selectedText;
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODETEXTINPUTDELEGATE_H_
