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

#include "appcode/shell/common/run_configuration.h"

#include <sstream>
#include <utility>

#include "appcode/assets/directory_asset_bundle.h"
#include "appcode/common/graphics/persistent_cache.h"
#include "appcode/fml/file.h"
#include "appcode/fml/unique_fd.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/runtime/isolate_configuration.h"

namespace appcode {

RunConfiguration RunConfiguration::InferFromSettings(
    const Settings& settings,
    const fml::RefPtr<fml::TaskRunner>& io_worker,
    IsolateLaunchType launch_type) {
  auto asset_manager = std::make_shared<AssetManager>();

  if (fml::UniqueFD::traits_type::IsValid(settings.assets_dir)) {
    asset_manager->PushBack(std::make_unique<DirectoryAssetBundle>(
        fml::Duplicate(settings.assets_dir), true));
  }

  asset_manager->PushBack(std::make_unique<DirectoryAssetBundle>(
      fml::OpenDirectory(settings.assets_path.c_str(), false,
                         fml::FilePermission::kRead),
      true));

  return {IsolateConfiguration::InferFromSettings(settings, asset_manager,
                                                  io_worker, launch_type),
          asset_manager};
}

RunConfiguration::RunConfiguration(
    std::unique_ptr<IsolateConfiguration> configuration)
    : RunConfiguration(std::move(configuration),
                       std::make_shared<AssetManager>()) {
#if !SLIMPELLER
  PersistentCache::SetAssetManager(asset_manager_);
#endif  //  !SLIMPELLER
}

RunConfiguration::RunConfiguration(
    std::unique_ptr<IsolateConfiguration> configuration,
    std::shared_ptr<AssetManager> asset_manager)
    : isolate_configuration_(std::move(configuration)),
      asset_manager_(std::move(asset_manager)) {
#if !SLIMPELLER
  PersistentCache::SetAssetManager(asset_manager_);
#endif  //  !SLIMPELLER
}

RunConfiguration::RunConfiguration(RunConfiguration&&) = default;

RunConfiguration::~RunConfiguration() = default;

bool RunConfiguration::IsValid() const {
  return asset_manager_ && isolate_configuration_;
}

bool RunConfiguration::AddAssetResolver(
    std::unique_ptr<AssetResolver> resolver) {
  if (!resolver || !resolver->IsValid()) {
    return false;
  }

  asset_manager_->PushBack(std::move(resolver));
  return true;
}

void RunConfiguration::SetEntrypoint(std::string entrypoint) {
  entrypoint_ = std::move(entrypoint);
}

void RunConfiguration::SetEntrypointAndLibrary(std::string entrypoint,
                                               std::string library) {
  SetEntrypoint(std::move(entrypoint));
  entrypoint_library_ = std::move(library);
}

void RunConfiguration::SetEntrypointArgs(
    std::vector<std::string> entrypoint_args) {
  entrypoint_args_ = std::move(entrypoint_args);
}

std::shared_ptr<AssetManager> RunConfiguration::GetAssetManager() const {
  return asset_manager_;
}

const std::string& RunConfiguration::GetEntrypoint() const {
  return entrypoint_;
}

const std::string& RunConfiguration::GetEntrypointLibrary() const {
  return entrypoint_library_;
}

const std::vector<std::string>& RunConfiguration::GetEntrypointArgs() const {
  return entrypoint_args_;
}

void RunConfiguration::SetEngineId(int64_t engine_id) {
  engine_id_ = engine_id;
}

// Engine identifier to be passed to the platform dispatcher.
std::optional<int64_t> RunConfiguration::GetEngineId() const {
  return engine_id_;
}

std::unique_ptr<IsolateConfiguration>
RunConfiguration::TakeIsolateConfiguration() {
  return std::move(isolate_configuration_);
}

}  // namespace appcode
