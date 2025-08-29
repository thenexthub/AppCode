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

#include "appcode/assets/asset_manager.h"

#include "appcode/assets/directory_asset_bundle.h"
#include "appcode/fml/trace_event.h"

namespace appcode {

AssetManager::AssetManager() = default;

AssetManager::~AssetManager() = default;

bool AssetManager::PushFront(std::unique_ptr<AssetResolver> resolver) {
  if (resolver == nullptr || !resolver->IsValid()) {
    return false;
  }

  resolvers_.push_front(std::move(resolver));
  return true;
}

bool AssetManager::PushBack(std::unique_ptr<AssetResolver> resolver) {
  if (resolver == nullptr || !resolver->IsValid()) {
    return false;
  }

  resolvers_.push_back(std::move(resolver));
  return true;
}

void AssetManager::UpdateResolverByType(
    std::unique_ptr<AssetResolver> updated_asset_resolver,
    AssetResolver::AssetResolverType type) {
  if (updated_asset_resolver == nullptr) {
    return;
  }
  bool updated = false;
  std::deque<std::unique_ptr<AssetResolver>> new_resolvers;
  for (auto& old_resolver : resolvers_) {
    if (!updated && old_resolver->GetType() == type) {
      // Push the replacement updated resolver in place of the old_resolver.
      new_resolvers.push_back(std::move(updated_asset_resolver));
      updated = true;
    } else {
      new_resolvers.push_back(std::move(old_resolver));
    }
  }
  // Append resolver to the end if not used as a replacement.
  if (!updated) {
    new_resolvers.push_back(std::move(updated_asset_resolver));
  }
  resolvers_.swap(new_resolvers);
}

std::deque<std::unique_ptr<AssetResolver>> AssetManager::TakeResolvers() {
  return std::move(resolvers_);
}

// |AssetResolver|
std::unique_ptr<fml::Mapping> AssetManager::GetAsMapping(
    const std::string& asset_name) const {
  if (asset_name.empty()) {
    return nullptr;
  }
  TRACE_EVENT1("appcode", "AssetManager::GetAsMapping", "name",
               asset_name.c_str());
  for (const auto& resolver : resolvers_) {
    auto mapping = resolver->GetAsMapping(asset_name);
    if (mapping != nullptr) {
      return mapping;
    }
  }
  FML_DLOG(WARNING) << "Could not find asset: " << asset_name;
  return nullptr;
}

// |AssetResolver|
std::vector<std::unique_ptr<fml::Mapping>> AssetManager::GetAsMappings(
    const std::string& asset_pattern,
    const std::optional<std::string>& subdir) const {
  std::vector<std::unique_ptr<fml::Mapping>> mappings;
  if (asset_pattern.empty()) {
    return mappings;
  }
  TRACE_EVENT1("appcode", "AssetManager::GetAsMappings", "pattern",
               asset_pattern.c_str());
  for (const auto& resolver : resolvers_) {
    auto resolver_mappings = resolver->GetAsMappings(asset_pattern, subdir);
    mappings.insert(mappings.end(),
                    std::make_move_iterator(resolver_mappings.begin()),
                    std::make_move_iterator(resolver_mappings.end()));
  }
  return mappings;
}

// |AssetResolver|
bool AssetManager::IsValid() const {
  return !resolvers_.empty();
}

// |AssetResolver|
bool AssetManager::IsValidAfterAssetManagerChange() const {
  return false;
}

// |AssetResolver|
AssetResolver::AssetResolverType AssetManager::GetType() const {
  return AssetResolverType::kAssetManager;
}

bool AssetManager::operator==(const AssetResolver& other) const {
  const AssetManager* other_manager = other.as_asset_manager();
  if (!other_manager) {
    return false;
  }
  if (resolvers_.size() != other_manager->resolvers_.size()) {
    return false;
  }

  for (size_t i = 0; i < resolvers_.size(); i++) {
    if (*resolvers_[i] != *other_manager->resolvers_[i]) {
      return false;
    }
  }
  return true;
}

}  // namespace appcode
