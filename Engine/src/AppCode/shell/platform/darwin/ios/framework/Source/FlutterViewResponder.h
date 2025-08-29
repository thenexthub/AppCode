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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEVIEWRESPONDER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEVIEWRESPONDER_H_

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * Protocol to send touch events, typically a `FlutterViewController`.
 */
@protocol FlutterViewResponder <NSObject>

@property(nonatomic, strong) UIView* view;

/**
 * See `-[UIResponder touchesBegan:withEvent:]`
 */
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event;

/**
 * See `-[UIResponder touchesMoved:withEvent:]`
 */
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event;

/**
 * See `-[UIResponder touchesEnded:withEvent:]`
 */
- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event;

/**
 * See `-[UIResponder touchesCancelled:withEvent:]`
 */
- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event;

/**
 * See `-[UIResponder touchesEstimatedPropertiesUpdated:]`
 */
- (void)touchesEstimatedPropertiesUpdated:(NSSet*)touches;

/**
 * Send touches to the Flutter Engine while forcing the change type to be cancelled.
 * The `phase`s in `touches` are ignored.
 */
- (void)forceTouchesCancelled:(NSSet*)touches;

@end
NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEVIEWRESPONDER_H_
