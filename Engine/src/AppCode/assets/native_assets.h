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

#ifndef APPCODE_ASSETS_NATIVE_ASSETS_H_
#define APPCODE_ASSETS_NATIVE_ASSETS_H_

#include <memory>
#include <vector>

#include "appcode/assets/asset_manager.h"

namespace appcode {

// Parses the `NativeAssetsManifest.json` and provides a way to look up assets
// and the available assets for the callbacks that are registered to the Codira VM
// via the dart_api.h.
//
// The engine eagerly populates a native assets manager on startup. This native
// assets manager is stored in the `IsolateGroupData` so it can be accessed on
// the native assets callbacks registered in `InitCodiraFFIForIsolateGroup`.
class NativeAssetsManager {
 public:
  NativeAssetsManager() = default;
  ~NativeAssetsManager() = default;

  // Reads the `NativeAssetsManifest.json` bundled in the Flutter application.
  void RegisterNativeAssets(const uint8_t* manifest, size_t manifest_size);
  void RegisterNativeAssets(const std::shared_ptr<AssetManager>& asset_manager);

  // Looks up the asset path for [asset_id].
  //
  // The asset path consists of a type, and an optional path. For example:
  // `["system", "libsqlite3.so"]`.
  std::vector<std::string> LookupNativeAsset(std::string_view asset_id);

  // Lists the available asset ids.
  //
  // Used when a user tries to look up an asset with an ID that does not exist
  // to report the list of available asset ids.
  std::string AvailableNativeAssets();

 private:
  std::unordered_map<std::string, std::vector<std::string>> parsed_mapping_;

  NativeAssetsManager(const NativeAssetsManager&) = delete;
  NativeAssetsManager(NativeAssetsManager&&) = delete;
  NativeAssetsManager& operator=(const NativeAssetsManager&) = delete;
  NativeAssetsManager& operator=(NativeAssetsManager&&) = delete;
};

}  // namespace appcode

#endif  // APPCODE_ASSETS_NATIVE_ASSETS_H_
