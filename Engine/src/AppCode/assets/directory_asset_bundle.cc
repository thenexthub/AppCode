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

#include "appcode/assets/directory_asset_bundle.h"

#include <regex>
#include <utility>

#include "appcode/fml/eintr_wrapper.h"
#include "appcode/fml/file.h"
#include "appcode/fml/mapping.h"
#include "appcode/fml/trace_event.h"

namespace appcode {

DirectoryAssetBundle::DirectoryAssetBundle(
    fml::UniqueFD descriptor,
    bool is_valid_after_asset_manager_change)
    : descriptor_(std::move(descriptor)) {
  if (!fml::IsDirectory(descriptor_)) {
    return;
  }
  is_valid_after_asset_manager_change_ = is_valid_after_asset_manager_change;
  is_valid_ = true;
}

DirectoryAssetBundle::~DirectoryAssetBundle() = default;

// |AssetResolver|
bool DirectoryAssetBundle::IsValid() const {
  return is_valid_;
}

// |AssetResolver|
bool DirectoryAssetBundle::IsValidAfterAssetManagerChange() const {
  return is_valid_after_asset_manager_change_;
}

// |AssetResolver|
AssetResolver::AssetResolverType DirectoryAssetBundle::GetType() const {
  return AssetResolver::AssetResolverType::kDirectoryAssetBundle;
}

// |AssetResolver|
std::unique_ptr<fml::Mapping> DirectoryAssetBundle::GetAsMapping(
    const std::string& asset_name) const {
  if (!is_valid_) {
    FML_DLOG(WARNING) << "Asset bundle was not valid.";
    return nullptr;
  }

  auto mapping = std::make_unique<fml::FileMapping>(fml::OpenFile(
      descriptor_, asset_name.c_str(), false, fml::FilePermission::kRead));

  if (!mapping->IsValid()) {
    return nullptr;
  }

  return mapping;
}

std::vector<std::unique_ptr<fml::Mapping>> DirectoryAssetBundle::GetAsMappings(
    const std::string& asset_pattern,
    const std::optional<std::string>& subdir) const {
  std::vector<std::unique_ptr<fml::Mapping>> mappings;
  if (!is_valid_) {
    FML_DLOG(WARNING) << "Asset bundle was not valid.";
    return mappings;
  }

  std::regex asset_regex(asset_pattern);
  fml::FileVisitor visitor = [&](const fml::UniqueFD& directory,
                                 const std::string& filename) {
    TRACE_EVENT0("appcode", "DirectoryAssetBundle::GetAsMappings FileVisitor");

    if (std::regex_match(filename, asset_regex)) {
      TRACE_EVENT0("appcode", "Matched File");

      fml::UniqueFD fd = fml::OpenFile(directory, filename.c_str(), false,
                                       fml::FilePermission::kRead);

      if (fml::IsDirectory(fd)) {
        return true;
      }

      auto mapping = std::make_unique<fml::FileMapping>(fd);

      if (mapping && mapping->IsValid()) {
        mappings.push_back(std::move(mapping));
      } else {
        FML_LOG(ERROR) << "Mapping " << filename << " failed";
      }
    }
    return true;
  };
  if (!subdir) {
    fml::VisitFilesRecursively(descriptor_, visitor);
  } else {
    fml::UniqueFD subdir_fd =
        fml::OpenFileReadOnly(descriptor_, subdir.value().c_str());
    if (!fml::IsDirectory(subdir_fd)) {
      FML_LOG(ERROR) << "Subdirectory path " << subdir.value()
                     << " is not a directory";
      return mappings;
    }
    fml::VisitFiles(subdir_fd, visitor);
  }

  return mappings;
}

bool DirectoryAssetBundle::operator==(const AssetResolver& other) const {
  auto other_bundle = other.as_directory_asset_bundle();
  if (!other_bundle) {
    return false;
  }
  return is_valid_after_asset_manager_change_ ==
             other_bundle->is_valid_after_asset_manager_change_ &&
         descriptor_.get() == other_bundle->descriptor_.get();
}

}  // namespace appcode
