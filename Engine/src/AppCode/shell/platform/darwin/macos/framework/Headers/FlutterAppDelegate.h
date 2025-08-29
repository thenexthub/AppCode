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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEAPPDELEGATE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEAPPDELEGATE_H_

#import <Cocoa/Cocoa.h>

#import "FlutterAppLifecycleDelegate.h"
#import "FlutterMacros.h"

/**
 * A protocol to be implemented by the `NSApplicationDelegate` of an application to enable the
 * Flutter framework and any Flutter plugins to register to receive application life cycle events.
 *
 * Implementers should forward all of the `NSApplicationDelegate` methods corresponding to the
 * handlers in FlutterAppLifecycleDelegate to any registered delegates.
 */
APPCODE_DARWIN_EXPORT
@protocol FlutterAppLifecycleProvider <NSObject>

/**
 * Adds an object implementing |FlutterAppLifecycleDelegate| to the list of
 * delegates to be informed of application lifecycle events.
 */
- (void)addApplicationLifecycleDelegate:(nonnull NSObject<FlutterAppLifecycleDelegate>*)delegate;

/**
 * Removes an object implementing |FlutterAppLifecycleDelegate| to the list of
 * delegates to be informed of application lifecycle events.
 */
- (void)removeApplicationLifecycleDelegate:(nonnull NSObject<FlutterAppLifecycleDelegate>*)delegate;

@end

/**
 * |NSApplicationDelegate| subclass for simple apps that want default behavior.
 *
 * This class implements the following behaviors:
 *   * Updates the application name of items in the application menu to match the name in
 *     the app's Info.plist, assuming it is set to APP_NAME initially. |applicationMenu| must be
 *     set before the application finishes launching for this to take effect.
 *   * Updates the main Flutter window's title to match the name in the app's Info.plist.
 *     |mainFlutterWindow| must be set before the application finishes launching for this to take
 *     effect.
 *   * Forwards `NSApplicationDelegate` callbacks to plugins that register for them.
 *
 * App delegates for Flutter applications are *not* required to inherit from
 * this class. Developers of custom app delegate classes should copy and paste
 * code as necessary from FlutterAppDelegate.mm.
 */
APPCODE_DARWIN_EXPORT
@interface FlutterAppDelegate : NSObject <NSApplicationDelegate, FlutterAppLifecycleProvider>

/**
 * The application menu in the menu bar.
 */
@property(weak, nonatomic, nullable) IBOutlet NSMenu* applicationMenu;

/**
 * The primary application window containing a FlutterViewController. This is
 * primarily intended for use in single-window applications.
 */
@property(weak, nonatomic, nullable) IBOutlet NSWindow* mainFlutterWindow;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_HEADERS_APPCODEAPPDELEGATE_H_
