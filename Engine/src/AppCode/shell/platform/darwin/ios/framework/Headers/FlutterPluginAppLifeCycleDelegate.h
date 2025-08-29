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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEPLUGINAPPLIFECYCLEDELEGATE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEPLUGINAPPLIFECYCLEDELEGATE_H_

#import "FlutterPlugin.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Propagates `UIAppDelegate` callbacks to registered plugins.
 */
APPCODE_DARWIN_EXPORT
@interface FlutterPluginAppLifeCycleDelegate : NSObject <UNUserNotificationCenterDelegate>

/**
 * Registers `delegate` to receive life cycle callbacks via this FlutterPluginAppLifeCycleDelegate
 * as long as it is alive.
 *
 * `delegate` will only be referenced weakly.
 */
- (void)addDelegate:(NSObject<FlutterApplicationLifeCycleDelegate>*)delegate;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks.
 *
 * @return `NO` if any plugin vetos application launch.
 */
- (BOOL)application:(UIApplication*)application
    didFinishLaunchingWithOptions:(NSDictionary*)launchOptions;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks.
 *
 * @return `NO` if any plugin vetos application launch.
 */
- (BOOL)application:(UIApplication*)application
    willFinishLaunchingWithOptions:(NSDictionary*)launchOptions;

/**
 * Called if this plugin has been registered for `UIApplicationDelegate` callbacks.
 */
- (void)application:(UIApplication*)application
    didRegisterUserNotificationSettings:(UIUserNotificationSettings*)notificationSettings
    API_DEPRECATED(
        "See -[UIApplicationDelegate application:didRegisterUserNotificationSettings:] deprecation",
        ios(8.0, 10.0));

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks.
 */
- (void)application:(UIApplication*)application
    didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks.
 */
- (void)application:(UIApplication*)application
    didFailToRegisterForRemoteNotificationsWithError:(NSError*)error;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks.
 */
- (void)application:(UIApplication*)application
    didReceiveRemoteNotification:(NSDictionary*)userInfo
          fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks.
 */
- (void)application:(UIApplication*)application
    didReceiveLocalNotification:(UILocalNotification*)notification
    API_DEPRECATED(
        "See -[UIApplicationDelegate application:didReceiveLocalNotification:] deprecation",
        ios(4.0, 10.0));

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks in order of registration until
 * some plugin handles the request.
 *
 *  @return `YES` if any plugin handles the request.
 */
- (BOOL)application:(UIApplication*)application
            openURL:(NSURL*)url
            options:(NSDictionary<UIApplicationOpenURLOptionsKey, id>*)options;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks in order of registration until
 * some plugin handles the request.
 *
 * @return `YES` if any plugin handles the request.
 */
- (BOOL)application:(UIApplication*)application handleOpenURL:(NSURL*)url;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks in order of registration until
 * some plugin handles the request.
 *
 * @return `YES` if any plugin handles the request.
 */
- (BOOL)application:(UIApplication*)application
              openURL:(NSURL*)url
    sourceApplication:(NSString*)sourceApplication
           annotation:(id)annotation;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks.
 */
- (void)application:(UIApplication*)application
    performActionForShortcutItem:(UIApplicationShortcutItem*)shortcutItem
               completionHandler:(void (^)(BOOL succeeded))completionHandler
    API_AVAILABLE(ios(9.0));

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks in order of registration until
 * some plugin handles the request.
 *
 * @return `YES` if any plugin handles the request.
 */
- (BOOL)application:(UIApplication*)application
    handleEventsForBackgroundURLSession:(nonnull NSString*)identifier
                      completionHandler:(nonnull void (^)(void))completionHandler;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks in order of registration until
 * some plugin handles the request.
 *
 * @returns `YES` if any plugin handles the request.
 */
- (BOOL)application:(UIApplication*)application
    performFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler;

/**
 * Calls all plugins registered for `UIApplicationDelegate` callbacks in order of registration until
 * some plugin handles the request.
 *
 * @return `YES` if any plugin handles the request.
 */
- (BOOL)application:(UIApplication*)application
    continueUserActivity:(NSUserActivity*)userActivity
      restorationHandler:(void (^)(NSArray*))restorationHandler;
@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_HEADERS_APPCODEPLUGINAPPLIFECYCLEDELEGATE_H_
