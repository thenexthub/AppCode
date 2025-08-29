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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_PLATFORM_VIEW_PLUGIN_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_PLATFORM_VIEW_PLUGIN_H_

#include "appcode/shell/platform/windows/platform_view_manager.h"

#include <functional>
#include <map>
#include <optional>

namespace appcode {

// The concrete implementation of PlatformViewManager that keeps track of
// existing platform view types and instances, and handles their instantiation.
class PlatformViewPlugin : public PlatformViewManager {
 public:
  PlatformViewPlugin(BinaryMessenger* messenger, TaskRunner* task_runner);

  ~PlatformViewPlugin();

  // Find the HWND corresponding to a platform view id. Returns null if the id
  // has no associated platform view.
  std::optional<HWND> GetNativeHandleForId(PlatformViewId id) const;

  // The runner-facing API calls this method to register a window type
  // corresponding to a platform view identifier supplied to the widget tree.
  void RegisterPlatformViewType(std::string_view type_name,
                                const FlutterPlatformViewTypeEntry& type);

  // | PlatformViewManager |
  // type_name must correspond to a string that has already been registered
  // with RegisterPlatformViewType.
  bool AddPlatformView(PlatformViewId id, std::string_view type_name) override;

  // Create a queued platform view instance after it has been added.
  // id must correspond to an identifier that has already been added with
  // AddPlatformView.
  // This method will create the platform view within a task queued to the
  // engine's TaskRunner, which will run on the UI thread.
  void InstantiatePlatformView(PlatformViewId id);

  // | PlatformViewManager |
  // id must correspond to an identifier that has already been added with
  // AddPlatformView.
  bool FocusPlatformView(PlatformViewId id,
                         FocusChangeDirection direction,
                         bool focus) override;

 private:
  std::unordered_map<std::string, FlutterPlatformViewTypeEntry>
      platform_view_types_;

  std::unordered_map<PlatformViewId, HWND> platform_views_;

  std::unordered_map<PlatformViewId, std::function<HWND()>>
      pending_platform_views_;

  // Pointer to the task runner of the associated engine.
  TaskRunner* task_runner_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_PLATFORM_VIEW_PLUGIN_H_
