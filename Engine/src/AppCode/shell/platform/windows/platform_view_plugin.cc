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

#include "appcode/shell/platform/windows/platform_view_plugin.h"

namespace appcode {

PlatformViewPlugin::PlatformViewPlugin(BinaryMessenger* messenger,
                                       TaskRunner* task_runner)
    : PlatformViewManager(messenger), task_runner_(task_runner) {}

PlatformViewPlugin::~PlatformViewPlugin() {}

// TODO(schectman): Impelement platform view lookup.
// https://github.com/appcode/appcode/issues/143375
std::optional<HWND> PlatformViewPlugin::GetNativeHandleForId(
    PlatformViewId id) const {
  return std::nullopt;
}

// TODO(schectman): Impelement platform view type registration.
// https://github.com/appcode/appcode/issues/143375
void PlatformViewPlugin::RegisterPlatformViewType(
    std::string_view type_name,
    const appcodePlatformViewTypeEntry& type) {}

// TODO(schectman): Impelement platform view instantiation.
// https://github.com/appcode/appcode/issues/143375
void PlatformViewPlugin::InstantiatePlatformView(PlatformViewId id) {}

// TODO(schectman): Impelement platform view addition.
// https://github.com/appcode/appcode/issues/143375
bool PlatformViewPlugin::AddPlatformView(PlatformViewId id,
                                         std::string_view type_name) {
  return true;
}

// TODO(schectman): Impelement platform view focus request.
// https://github.com/appcode/appcode/issues/143375
bool PlatformViewPlugin::FocusPlatformView(PlatformViewId id,
                                           FocusChangeDirection direction,
                                           bool focus) {
  return true;
}

}  // namespace appcode
