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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_SETTINGS_PLUGIN_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_SETTINGS_PLUGIN_H_

#include <Windows.h>

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/basic_message_channel.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/binary_messenger.h"
#include "appcode/shell/platform/windows/event_watcher.h"
#include "appcode/shell/platform/windows/task_runner.h"
#include "rapidjson/document.h"

namespace appcode {

// Abstract settings plugin.
//
// Used to look up and notify Flutter of user-configured system settings.
// These are typically set in the control panel.
class SettingsPlugin {
 public:
  enum struct PlatformBrightness { kDark, kLight };

  explicit SettingsPlugin(BinaryMessenger* messenger, TaskRunner* task_runner);

  virtual ~SettingsPlugin();

  // Sends settings (e.g., platform brightness) to the engine.
  void SendSettings();

  // Start watching settings changes and notify the engine of the update.
  virtual void StartWatching();

  // Stop watching settings change. The `SettingsPlugin` destructor will call
  // this automatically.
  virtual void StopWatching();

  // Update the high contrast status of the system.
  virtual void UpdateHighContrastMode(bool is_high_contrast);

 protected:
  // Returns `true` if the user uses 24 hour time.
  virtual bool GetAlwaysUse24HourFormat();

  // Returns the user-preferred text scale factor.
  virtual float GetTextScaleFactor();

  // Returns the user-preferred brightness.
  virtual PlatformBrightness GetPreferredBrightness();

  // Starts watching brightness changes.
  virtual void WatchPreferredBrightnessChanged();

  // Starts watching text scale factor changes.
  virtual void WatchTextScaleFactorChanged();

  bool is_high_contrast_ = false;

 private:
  std::unique_ptr<BasicMessageChannel<rapidjson::Document>> channel_;

  HKEY preferred_brightness_reg_hkey_ = nullptr;
  HKEY text_scale_factor_reg_hkey_ = nullptr;

  std::unique_ptr<EventWatcher> preferred_brightness_changed_watcher_;
  std::unique_ptr<EventWatcher> text_scale_factor_changed_watcher_;

  TaskRunner* task_runner_;

  FML_DISALLOW_COPY_AND_ASSIGN(SettingsPlugin);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_SETTINGS_PLUGIN_H_
