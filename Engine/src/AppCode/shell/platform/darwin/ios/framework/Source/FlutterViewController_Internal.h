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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEVIEWCONTROLLER_INTERNAL_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEVIEWCONTROLLER_INTERNAL_H_

#import "appcode/shell/platform/darwin/ios/framework/Headers/FlutterViewController.h"

#include "appcode/fml/time/time_point.h"
#import "appcode/shell/platform/darwin/ios/InternalFlutterSwift/InternalFlutterSwift.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterKeySecondaryResponder.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterKeyboardManager.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterPlatformViewsController.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterRestorationPlugin.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterTextInputPlugin.h"
#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterViewResponder.h"

namespace appcode {
class PlatformViewsController;
}

APPCODE_DARWIN_EXPORT
// NOLINTNEXTLINE(readability-identifier-naming)
extern NSNotificationName const FlutterViewControllerWillDealloc;

APPCODE_DARWIN_EXPORT
// NOLINTNEXTLINE(readability-identifier-naming)
extern NSNotificationName const FlutterViewControllerHideHomeIndicator;

APPCODE_DARWIN_EXPORT
// NOLINTNEXTLINE(readability-identifier-naming)
extern NSNotificationName const FlutterViewControllerShowHomeIndicator;

typedef NS_ENUM(NSInteger, FlutterKeyboardMode) {
  // NOLINTBEGIN(readability-identifier-naming)
  FlutterKeyboardModeHidden = 0,
  FlutterKeyboardModeDocked = 1,
  FlutterKeyboardModeFloating = 2,
  // NOLINTEND(readability-identifier-naming)
};

typedef void (^FlutterKeyboardAnimationCallback)(fml::TimePoint);

@interface FlutterViewController () <FlutterViewResponder>

@property(class, nonatomic, readonly) BOOL accessibilityIsOnOffSwitchLabelsEnabled;
@property(nonatomic, readonly) BOOL isPresentingViewController;
@property(nonatomic, readonly) BOOL isVoiceOverRunning;
@property(nonatomic, strong) FlutterKeyboardManager* keyboardManager;

/**
 * @brief Whether the status bar is preferred hidden.
 *
 *        This overrides the |UIViewController:prefersStatusBarHidden|.
 *        This is ignored when `UIViewControllerBasedStatusBarAppearance` in info.plist
 *        of the app project is `false`.
 */
@property(nonatomic, assign, readwrite) BOOL prefersStatusBarHidden;

@property(nonatomic, readonly) FlutterPlatformViewsController* platformViewsController;

- (FlutterRestorationPlugin*)restorationPlugin;
- (FlutterTextInputPlugin*)textInputPlugin;

// Accepts keypress events, and then calls |nextAction| if the event was not
// handled.
- (void)handlePressEvent:(FlutterUIPressProxy*)press
              nextAction:(void (^)())nextAction API_AVAILABLE(ios(13.4));
- (void)sendDeepLinkToFramework:(NSURL*)url completionHandler:(void (^)(BOOL success))completion;
- (void)addInternalPlugins;
- (void)deregisterNotifications;
- (int32_t)accessibilityFlags;

- (BOOL)supportsShowingSystemContextMenu;
- (BOOL)stateIsActive;
- (BOOL)stateIsBackground;
@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_APPCODEVIEWCONTROLLER_INTERNAL_H_
