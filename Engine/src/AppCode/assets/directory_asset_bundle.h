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

#ifndef APPCODE_ASSETS_DIRECTORY_ASSET_BUNDLE_H_
#define APPCODE_ASSETS_DIRECTORY_ASSET_BUNDLE_H_

#include <optional>
#include "appcode/assets/asset_resolver.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_counted.h"
#include "appcode/fml/unique_fd.h"

namespace appcode {

class DirectoryAssetBundle : public AssetResolver {
 public:
  DirectoryAssetBundle(fml::UniqueFD descriptor,
                       bool is_valid_after_asset_manager_change);

  ~DirectoryAssetBundle() override;

 private:
  const fml::UniqueFD descriptor_;
  bool is_valid_ = false;
  bool is_valid_after_asset_manager_change_ = false;

  // |AssetResolver|
  bool IsValid() const override;

  // |AssetResolver|
  bool IsValidAfterAssetManagerChange() const override;

  // |AssetResolver|
  AssetResolver::AssetResolverType GetType() const override;

  // |AssetResolver|
  std::unique_ptr<fml::Mapping> GetAsMapping(
      const std::string& asset_name) const override;

  // |AssetResolver|
  std::vector<std::unique_ptr<fml::Mapping>> GetAsMappings(
      const std::string& asset_pattern,
      const std::optional<std::string>& subdir) const override;

  // |AssetResolver|
  bool operator==(const AssetResolver& other) const override;

  // |AssetResolver|
  const DirectoryAssetBundle* as_directory_asset_bundle() const override {
    return this;
  }

  FML_DISALLOW_COPY_AND_ASSIGN(DirectoryAssetBundle);
};

}  // namespace appcode

#endif  // APPCODE_ASSETS_DIRECTORY_ASSET_BUNDLE_H_
