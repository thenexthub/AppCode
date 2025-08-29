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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODESEMANTICSSCROLLVIEW_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODESEMANTICSSCROLLVIEW_H_

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN
@class SemanticsObject;

/**
 * A UIScrollView to represent Flutter scrollable in iOS accessibility
 * services.
 *
 * This class is hidden from the user and can't be interacted with. It
 * sends all of selector calls from accessibility services to the
 * owner SemanticsObject.
 */
@interface FlutterSemanticsScrollView : UIScrollView <UIScrollViewDelegate>

@property(nonatomic, weak, nullable) SemanticsObject* semanticsObject;

/// Whether this scroll view's content offset is actively being updated by UIKit
/// or other the system services.
///
/// This flag is set by the `FlutterSemanticsScrollView` itself, typically in
/// one of the `UIScrollViewDelegate` methods.
///
/// When this flag is true, the `SemanticsObject` implementation ignores all
/// content offset updates coming from the Flutter framework, to prevent
/// potential feedback loops (especially when the framework is only echoing
/// the new content offset back to this scroll view).
///
/// For example, to scroll a scrollable container with iOS full keyboard access,
/// the iOS focus system uses a display link to scroll the container to the
/// desired offset animatedly. If the user changes the scroll offset during the
/// animation, the display link will be invalidated and the scrolling animation
/// will be interrupted. For simplicity, content offset updates coming from the
/// framework will be ignored in the relatively short animation duration (~1s),
/// allowing the scrolling animation to finish.
@property(nonatomic, readonly) BOOL isDoingSystemScrolling;

- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithFrame:(CGRect)frame NS_UNAVAILABLE;
- (instancetype)initWithCoder:(NSCoder*)coder NS_UNAVAILABLE;
- (instancetype)initWithSemanticsObject:(SemanticsObject*)semanticsObject;

@end
NS_ASSUME_NONNULL_END
#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODESEMANTICSSCROLLVIEW_H_
