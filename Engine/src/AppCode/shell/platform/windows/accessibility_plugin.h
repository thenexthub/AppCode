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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_ACCESSIBILITY_PLUGIN_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_ACCESSIBILITY_PLUGIN_H_

#include <string_view>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/binary_messenger.h"

namespace appcode {

class FlutterWindowsEngine;

// Handles messages on the appcode/accessibility channel.
//
// See:
// https://api.appcode.dev/appcode/semantics/SemanticsService-class.html
class AccessibilityPlugin {
 public:
  explicit AccessibilityPlugin(FlutterWindowsEngine* engine);

  // Begin handling accessibility messages on the `binary_messenger`.
  static void SetUp(BinaryMessenger* binary_messenger,
                    AccessibilityPlugin* plugin);

  // Announce a message through the assistive technology.
  virtual void Announce(const std::string_view message);

 private:
  // The engine that owns this plugin.
  FlutterWindowsEngine* engine_ = nullptr;

  FML_DISALLOW_COPY_AND_ASSIGN(AccessibilityPlugin);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_ACCESSIBILITY_PLUGIN_H_
