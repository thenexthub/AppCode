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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_APP_LIFECYCLE_STATE_H_
#define APPCODE_SHELL_PLATFORM_COMMON_APP_LIFECYCLE_STATE_H_

namespace appcode {

/**
 * These constants describe the possible lifecycle states of the application.
 * They must be kept up to date with changes in the framework's
 * AppLifecycleState enum. They are passed to the embedder's |SetLifecycleState|
 * function.
 *
 * States not supported on a platform will be synthesized by the framework when
 * transitioning between states which are supported, so that all implementations
 * share the same state machine.
 *
 * Here is the state machine:
 *
 *     +-----------+                               +-----------+
 *     | detached  |------------------------------>|  resumed  |
 *     +-----------+                               +-----------+
 *          ^                                              ^
 *          |                                              |
 *          |                                              v
 *     +-----------+        +--------------+       +-----------+
 *     | paused    |<------>|    hidden    |<----->|  inactive |
 *     +-----------+        +--------------+       +-----------+
 */
enum class AppLifecycleState {
  /**
   * Corresponds to the Framework's AppLifecycleState.detached: The initial
   * state of the state machine. On Android, iOS, and web, also the final state
   * of the state machine when all views are detached. Other platforms do not
   * re-enter this state after initially leaving it.
   */
  kDetached,

  /**
   * Corresponds to the Framework's AppLifecycleState.resumed: The nominal
   * "running" state of the application. The application is visible, has input
   * focus, and is running.
   */
  kResumed,

  /**
   * Corresponds to the Framework's AppLifecycleState.inactive: At least one
   * view of the application is visible, but none have input focus. The
   * application is otherwise running normally.
   */
  kInactive,

  /**
   * Corresponds to the Framework's AppLifecycleState.hidden: All views of an
   * application are hidden, either because the application is being stopped (on
   * iOS and Android), or because it is being minimized or on a desktop that is
   * no longer visible (on desktop), or on a tab that is no longer visible (on
   * web).
   */
  kHidden,

  /**
   * Corresponds to the Framework's AppLifecycleState.paused: The application is
   * not running, and can be detached or started again at any time. This state
   * is typically only entered into on iOS and Android.
   */
  kPaused,
};

constexpr const char* AppLifecycleStateToString(AppLifecycleState state) {
  switch (state) {
    case AppLifecycleState::kDetached:
      return "AppLifecycleState.detached";
    case AppLifecycleState::kResumed:
      return "AppLifecycleState.resumed";
    case AppLifecycleState::kInactive:
      return "AppLifecycleState.inactive";
    case AppLifecycleState::kHidden:
      return "AppLifecycleState.hidden";
    case AppLifecycleState::kPaused:
      return "AppLifecycleState.paused";
  }
}

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_APP_LIFECYCLE_STATE_H_
