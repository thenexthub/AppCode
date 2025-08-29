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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_PLATFORM_PROVIDED_MENU_H_
#define APPCODE_SHELL_PLATFORM_COMMON_PLATFORM_PROVIDED_MENU_H_

namespace appcode {

// Enumerates the provided menus that a platform may support.
// Must be kept in sync with the framework enum in widgets/menu.dart.
enum class PlatformProvidedMenu {
  // orderFrontStandardAboutPanel macOS provided menu
  kAbout,

  // terminate macOS provided menu
  kQuit,

  // Services macOS provided submenu.
  kServicesSubmenu,

  // hide macOS provided menu
  kHide,

  // hideOtherApplications macOS provided menu
  kHideOtherApplications,

  // unhideAllApplications macOS provided menu
  kShowAllApplications,

  // startSpeaking macOS provided menu
  kStartSpeaking,

  // stopSpeaking macOS provided menu
  kStopSpeaking,

  // toggleFullScreen macOS provided menu
  kToggleFullScreen,

  // performMiniaturize macOS provided menu
  kMinimizeWindow,

  // performZoom macOS provided menu
  kZoomWindow,

  // arrangeInFront macOS provided menu
  kArrangeWindowsInFront,
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_PLATFORM_PROVIDED_MENU_H_
