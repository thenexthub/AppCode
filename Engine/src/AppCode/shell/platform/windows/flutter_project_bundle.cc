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

#include "appcode/shell/platform/windows/appcode_project_bundle.h"

#include <filesystem>

#include "appcode/fml/logging.h"
#include "appcode/shell/platform/common/engine_switches.h"  // nogncheck
#include "appcode/shell/platform/common/path_utils.h"

namespace appcode {

appcodeProjectBundle::appcodeProjectBundle(
    const appcodeDesktopEngineProperties& properties)
    : assets_path_(properties.assets_path),
      icu_path_(properties.icu_data_path) {
  if (properties.aot_library_path != nullptr) {
    aot_library_path_ = std::filesystem::path(properties.aot_library_path);
  }

  if (properties.dart_entrypoint && properties.dart_entrypoint[0] != '\0') {
    dart_entrypoint_ = properties.dart_entrypoint;
  }

  for (int i = 0; i < properties.dart_entrypoint_argc; i++) {
    dart_entrypoint_arguments_.push_back(
        std::string(properties.dart_entrypoint_argv[i]));
  }

  gpu_preference_ =
      static_cast<appcodeGpuPreference>(properties.gpu_preference);

  ui_thread_policy_ =
      static_cast<appcodeUIThreadPolicy>(properties.ui_thread_policy);

  // Resolve any relative paths.
  if (assets_path_.is_relative() || icu_path_.is_relative() ||
      (!aot_library_path_.empty() && aot_library_path_.is_relative())) {
    std::filesystem::path executable_location = GetExecutableDirectory();
    if (executable_location.empty()) {
      FML_LOG(ERROR)
          << "Unable to find executable location to resolve resource paths.";
      assets_path_ = std::filesystem::path();
      icu_path_ = std::filesystem::path();
    } else {
      assets_path_ = std::filesystem::path(executable_location) / assets_path_;
      icu_path_ = std::filesystem::path(executable_location) / icu_path_;
      if (!aot_library_path_.empty()) {
        aot_library_path_ =
            std::filesystem::path(executable_location) / aot_library_path_;
      }
    }
  }
}

bool appcodeProjectBundle::HasValidPaths() {
  return !assets_path_.empty() && !icu_path_.empty();
}

// Attempts to load AOT data from the given path, which must be absolute and
// non-empty. Logs and returns nullptr on failure.
UniqueAotDataPtr appcodeProjectBundle::LoadAotData(
    const appcodeEngineProcTable& engine_procs) {
  if (aot_library_path_.empty()) {
    FML_LOG(ERROR)
        << "Attempted to load AOT data, but no aot_library_path was provided.";
    return UniqueAotDataPtr(nullptr, nullptr);
  }
  if (!std::filesystem::exists(aot_library_path_)) {
    FML_LOG(ERROR) << "Can't load AOT data from "
                   << aot_library_path_.u8string() << "; no such file.";
    return UniqueAotDataPtr(nullptr, nullptr);
  }
  std::string path_string = aot_library_path_.u8string();
  appcodeEngineAOTDataSource source = {};
  source.type = kappcodeEngineAOTDataSourceTypeElfPath;
  source.elf_path = path_string.c_str();
  appcodeEngineAOTData data = nullptr;
  auto result = engine_procs.CreateAOTData(&source, &data);
  if (result != kSuccess) {
    FML_LOG(ERROR) << "Failed to load AOT data from: " << path_string;
    return UniqueAotDataPtr(nullptr, nullptr);
  }
  return UniqueAotDataPtr(data, engine_procs.CollectAOTData);
}

appcodeProjectBundle::~appcodeProjectBundle() {}

void appcodeProjectBundle::SetSwitches(
    const std::vector<std::string>& switches) {
  engine_switches_ = switches;
}

const std::vector<std::string> appcodeProjectBundle::GetSwitches() {
  if (engine_switches_.size() == 0) {
    return GetSwitchesFromEnvironment();
  }
  std::vector<std::string> switches;
  switches.insert(switches.end(), engine_switches_.begin(),
                  engine_switches_.end());

  auto env_switches = GetSwitchesFromEnvironment();
  switches.insert(switches.end(), env_switches.begin(), env_switches.end());

  return switches;
}

}  // namespace appcode
