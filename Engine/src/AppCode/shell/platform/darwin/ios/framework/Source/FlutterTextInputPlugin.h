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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODETEXTINPUTPLUGIN_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODETEXTINPUTPLUGIN_H_

#import <UIKit/UIKit.h>

#import "appcode/shell/platform/common/text_editing_delta.h"
#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterChannels.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterIndirectScribbleDelegate.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterKeySecondaryResponder.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterTextInputDelegate.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterViewResponder.h"

typedef NS_ENUM(NSInteger, FlutterScribbleFocusStatus) {
  // NOLINTBEGIN(readability-identifier-naming)
  FlutterScribbleFocusStatusUnfocused,
  FlutterScribbleFocusStatusFocusing,
  FlutterScribbleFocusStatusFocused,
  // NOLINTEND(readability-identifier-naming)
};

typedef NS_ENUM(NSInteger, FlutterScribbleInteractionStatus) {
  // NOLINTBEGIN(readability-identifier-naming)
  FlutterScribbleInteractionStatusNone,
  FlutterScribbleInteractionStatusStarted,
  FlutterScribbleInteractionStatusEnding,
  // NOLINTEND(readability-identifier-naming)
};

@interface FlutterTextInputPlugin
    : NSObject <FlutterKeySecondaryResponder, UIIndirectScribbleInteractionDelegate>

@property(nonatomic, weak) UIViewController* viewController;
@property(nonatomic, weak) id<FlutterIndirectScribbleDelegate> indirectScribbleDelegate;
@property(nonatomic, strong)
    NSMutableDictionary<UIScribbleElementIdentifier, NSValue*>* scribbleElements;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

- (instancetype)initWithDelegate:(id<FlutterTextInputDelegate>)textInputDelegate
    NS_DESIGNATED_INITIALIZER;

- (void)handleMethodCall:(FlutterMethodCall*)call result:(FlutterResult)result;

/**
 * Reset the text input plugin to prepare for a hot restart.
 *
 * This hides the software keyboard and text editing context menu.
 */
- (void)reset;

/**
 * The `UITextInput` implementation used to control text entry.
 *
 * This is used by `AccessibilityBridge` to forward interactions with iOS'
 * accessibility system.
 */
- (UIView<UITextInput>*)textInputView;

/**
 * These are used by the UIIndirectScribbleInteractionDelegate methods to handle focusing on the
 * correct element.
 */
- (void)setUpIndirectScribbleInteraction:(id<FlutterViewResponder>)viewResponder;
- (void)resetViewResponder;
- (BOOL)showEditMenu:(NSDictionary*)args API_AVAILABLE(ios(16.0));
- (void)hideEditMenu API_AVAILABLE(ios(16.0));

@end

/** An indexed position in the buffer of a Flutter text editing widget. */
@interface FlutterTextPosition : UITextPosition

@property(nonatomic, readonly) NSUInteger index;
@property(nonatomic, readonly) UITextStorageDirection affinity;

+ (instancetype)positionWithIndex:(NSUInteger)index;
+ (instancetype)positionWithIndex:(NSUInteger)index affinity:(UITextStorageDirection)affinity;
- (instancetype)initWithIndex:(NSUInteger)index affinity:(UITextStorageDirection)affinity;

@end

/** A range of text in the buffer of a Flutter text editing widget. */
@interface FlutterTextRange : UITextRange <NSCopying>

@property(nonatomic, readonly) NSRange range;

+ (instancetype)rangeWithNSRange:(NSRange)range;

@end

/** A tokenizer used by `FlutterTextInputView` to customize string parsing. */
@interface FlutterTokenizer : UITextInputStringTokenizer
@end

@interface FlutterTextSelectionRect : UITextSelectionRect

@property(nonatomic, assign) CGRect rect;
@property(nonatomic) NSUInteger position;
@property(nonatomic, assign) NSWritingDirection writingDirection;
@property(nonatomic) BOOL containsStart;
@property(nonatomic) BOOL containsEnd;
@property(nonatomic) BOOL isVertical;

+ (instancetype)selectionRectWithRectAndInfo:(CGRect)rect
                                    position:(NSUInteger)position
                            writingDirection:(NSWritingDirection)writingDirection
                               containsStart:(BOOL)containsStart
                                 containsEnd:(BOOL)containsEnd
                                  isVertical:(BOOL)isVertical;

+ (instancetype)selectionRectWithRect:(CGRect)rect position:(NSUInteger)position;

+ (instancetype)selectionRectWithRect:(CGRect)rect
                             position:(NSUInteger)position
                     writingDirection:(NSWritingDirection)writingDirection;

- (instancetype)initWithRectAndInfo:(CGRect)rect
                           position:(NSUInteger)position
                   writingDirection:(NSWritingDirection)writingDirection
                      containsStart:(BOOL)containsStart
                        containsEnd:(BOOL)containsEnd
                         isVertical:(BOOL)isVertical;

- (instancetype)init NS_UNAVAILABLE;

- (BOOL)isRTL;
@end

API_AVAILABLE(ios(13.0)) @interface FlutterTextPlaceholder : UITextPlaceholder
@end

#if APPCODE_RUNTIME_MODE == APPCODE_RUNTIME_MODE_DEBUG
APPCODE_DARWIN_EXPORT
#endif
@interface FlutterTextInputView
    : UIView <UITextInput, UIScribbleInteractionDelegate, UIEditMenuInteractionDelegate>

// UITextInput
@property(nonatomic, readonly) NSMutableString* text;
@property(readwrite, copy) UITextRange* selectedTextRange;
@property(nonatomic, strong) UITextRange* markedTextRange;
@property(nonatomic, copy) NSDictionary* markedTextStyle;
@property(nonatomic, weak) id<UITextInputDelegate> inputDelegate;
@property(nonatomic, strong) NSMutableArray* pendingDeltas;

// UITextInputTraits
@property(nonatomic) UITextAutocapitalizationType autocapitalizationType;
@property(nonatomic) UITextAutocorrectionType autocorrectionType;
@property(nonatomic) UITextSpellCheckingType spellCheckingType;
@property(nonatomic) BOOL enablesReturnKeyAutomatically;
@property(nonatomic) UIKeyboardAppearance keyboardAppearance;
@property(nonatomic) UIKeyboardType keyboardType;
@property(nonatomic) UIReturnKeyType returnKeyType;
@property(nonatomic, getter=isSecureTextEntry) BOOL secureTextEntry;
@property(nonatomic, getter=isEnableDeltaModel) BOOL enableDeltaModel;
@property(nonatomic) UITextSmartQuotesType smartQuotesType API_AVAILABLE(ios(11.0));
@property(nonatomic) UITextSmartDashesType smartDashesType API_AVAILABLE(ios(11.0));
@property(nonatomic, copy) UITextContentType textContentType API_AVAILABLE(ios(10.0));

@property(nonatomic, weak) UIAccessibilityElement* backingTextInputAccessibilityObject;

// Scribble Support
@property(nonatomic, weak) id<FlutterViewResponder> viewResponder;
@property(nonatomic) FlutterScribbleFocusStatus scribbleFocusStatus;
@property(nonatomic, strong) NSArray<FlutterTextSelectionRect*>* selectionRects;

@property(nonatomic, strong) UIEditMenuInteraction* editMenuInteraction API_AVAILABLE(ios(16.0));
- (void)resetScribbleInteractionStatusIfEnding;
- (BOOL)isScribbleAvailable;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)initWithCoder:(NSCoder*)aDecoder NS_UNAVAILABLE;
- (instancetype)initWithFrame:(CGRect)frame NS_UNAVAILABLE;
- (instancetype)initWithOwner:(FlutterTextInputPlugin*)textInputPlugin NS_DESIGNATED_INITIALIZER;

// TODO(louisehsu): These are being exposed to support Share in FlutterPlatformPlugin
// Consider moving that feature into FlutterTextInputPlugin to avoid exposing extra methods
- (CGRect)localRectFromFrameworkTransform:(CGRect)incomingRect;
- (CGRect)caretRectForPosition:(UITextPosition*)position;
@end

@interface UIView (FindFirstResponder)
@property(nonatomic, readonly) id appcodeFirstResponder;
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODETEXTINPUTPLUGIN_H_
