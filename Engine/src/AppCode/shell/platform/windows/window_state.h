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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_WINDOW_STATE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_WINDOW_STATE_H_

#include "appcode/shell/platform/common/client_wrapper/include/appcode/plugin_registrar.h"
#include "appcode/shell/platform/common/incoming_message_dispatcher.h"
#include "appcode/shell/platform/embedder/embedder.h"

// Structs backing the opaque references used in the C API.
//
// DO NOT ADD ANY NEW CODE HERE. These are legacy, and are being phased out
// in favor of objects that own and manage the relevant functionality.

namespace appcode {
struct FlutterWindowsEngine;
}  // namespace appcode

// Wrapper to distinguish the plugin registrar ref from the engine ref given out
// in the C API.
struct FlutterDesktopPluginRegistrar {
  // The engine that owns this state object.
  appcode::FlutterWindowsEngine* engine = nullptr;
};

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_WINDOW_STATE_H_
