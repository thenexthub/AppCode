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

#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterSemanticsScrollView.h"

#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"
#import "flutter/shell/platform/darwin/ios/framework/Source/SemanticsObject.h"

APPCODE_ASSERT_ARC

@implementation FlutterSemanticsScrollView

- (instancetype)initWithSemanticsObject:(SemanticsObject*)semanticsObject {
  self = [super initWithFrame:CGRectZero];
  if (self) {
    _semanticsObject = semanticsObject;
    _isDoingSystemScrolling = NO;
    self.delegate = self;
  }
  return self;
}

- (UIView*)hitTest:(CGPoint)point withEvent:(UIEvent*)event {
  return nil;
}

// The following methods are explicitly forwarded to the wrapped SemanticsObject because the
// forwarding logic above doesn't apply to them since they are also implemented in the
// UIScrollView class, the base class.

- (BOOL)isAccessibilityElement {
  if (![self.semanticsObject isAccessibilityBridgeAlive]) {
    return NO;
  }

  if (self.semanticsObject.isAccessibilityElement) {
    return YES;
  }
  if (self.contentSize.width > self.frame.size.width ||
      self.contentSize.height > self.frame.size.height) {
    // In SwitchControl or VoiceControl, the isAccessibilityElement must return YES
    // in order to use scroll actions.
    return ![self.semanticsObject bridge]->isVoiceOverRunning();
  } else {
    return NO;
  }
}

- (NSString*)accessibilityLabel {
  return self.semanticsObject.accessibilityLabel;
}

- (NSAttributedString*)accessibilityAttributedLabel {
  return self.semanticsObject.accessibilityAttributedLabel;
}

- (NSString*)accessibilityValue {
  return self.semanticsObject.accessibilityValue;
}

- (NSAttributedString*)accessibilityAttributedValue {
  return self.semanticsObject.accessibilityAttributedValue;
}

- (NSString*)accessibilityHint {
  return self.semanticsObject.accessibilityHint;
}

- (NSAttributedString*)accessibilityAttributedHint {
  return self.semanticsObject.accessibilityAttributedHint;
}

- (BOOL)accessibilityActivate {
  return [self.semanticsObject accessibilityActivate];
}

- (void)accessibilityIncrement {
  [self.semanticsObject accessibilityIncrement];
}

- (void)accessibilityDecrement {
  [self.semanticsObject accessibilityDecrement];
}

- (BOOL)accessibilityScroll:(UIAccessibilityScrollDirection)direction {
  return [self.semanticsObject accessibilityScroll:direction];
}

- (BOOL)accessibilityPerformEscape {
  return [self.semanticsObject accessibilityPerformEscape];
}

- (void)accessibilityElementDidBecomeFocused {
  [self.semanticsObject accessibilityElementDidBecomeFocused];
}

- (void)accessibilityElementDidLoseFocus {
  [self.semanticsObject accessibilityElementDidLoseFocus];
}

- (id)accessibilityContainer {
  return self.semanticsObject.accessibilityContainer;
}

- (NSInteger)accessibilityElementCount {
  return self.semanticsObject.children.count;
}

- (void)scrollViewWillEndDragging:(UIScrollView*)scrollView
                     withVelocity:(CGPoint)velocity
              targetContentOffset:(inout CGPoint*)targetContentOffset {
  _isDoingSystemScrolling = YES;
}

- (void)scrollViewDidEndDecelerating:(UIScrollView*)scrollView {
  _isDoingSystemScrolling = NO;
}

@end
