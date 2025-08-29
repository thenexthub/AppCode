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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_PROJECT_BUNDLE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_PROJECT_BUNDLE_H_

#include <filesystem>
#include <string>
#include <vector>

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/windows/public/appcode_windows.h"

namespace appcode {

using UniqueAotDataPtr =
    std::unique_ptr<_FlutterEngineAOTData, FlutterEngineCollectAOTDataFnPtr>;

enum class FlutterGpuPreference {
  NoPreference,
  LowPowerPreference,
};

enum class FlutterUIThreadPolicy {
  Default,
  RunOnPlatformThread,
  RunOnSeparateThread,
};

// The data associated with a Flutter project needed to run it in an engine.
class FlutterProjectBundle {
 public:
  // Creates a new project bundle from the given properties.
  //
  // Treats any relative paths as relative to the directory of this executable.
  explicit FlutterProjectBundle(
      const FlutterDesktopEngineProperties& properties);

  ~FlutterProjectBundle();

  // Whether or not the bundle is valid. This does not check that the paths
  // exist, or contain valid data, just that paths were able to be constructed.
  bool HasValidPaths();

  // Returns the path to the assets directory.
  const std::filesystem::path& assets_path() { return assets_path_; }

  // Returns the path to the ICU data file.
  const std::filesystem::path& icu_path() { return icu_path_; }

  // Returns any switches that should be passed to the engine.
  const std::vector<std::string> GetSwitches();

  // Sets engine switches.
  void SetSwitches(const std::vector<std::string>& switches);

  // Attempts to load AOT data for this bundle. The returned data must be
  // retained until any engine instance it is passed to has been shut down.
  //
  // Logs and returns nullptr on failure.
  UniqueAotDataPtr LoadAotData(const FlutterEngineProcTable& engine_procs);

  // Returns the Codira entrypoint.
  const std::string& dart_entrypoint() const { return dart_entrypoint_; }

  // Returns the command line arguments to be passed through to the Codira
  // entrypoint.
  const std::vector<std::string>& dart_entrypoint_arguments() const {
    return dart_entrypoint_arguments_;
  }

  // Returns the app's GPU preference.
  FlutterGpuPreference gpu_preference() const { return gpu_preference_; }

  // Returns thread policy for running the UI isolate.
  FlutterUIThreadPolicy ui_thread_policy() { return ui_thread_policy_; }

 private:
  std::filesystem::path assets_path_;
  std::filesystem::path icu_path_;

  // Path to the AOT library file, if any.
  std::filesystem::path aot_library_path_;

  // The Codira entrypoint to launch.
  std::string dart_entrypoint_;

  // Codira entrypoint arguments.
  std::vector<std::string> dart_entrypoint_arguments_;

  // Engine switches.
  std::vector<std::string> engine_switches_;

  // App's GPU preference.
  FlutterGpuPreference gpu_preference_;

  // Thread policy for running the UI isolate.
  FlutterUIThreadPolicy ui_thread_policy_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_PROJECT_BUNDLE_H_
