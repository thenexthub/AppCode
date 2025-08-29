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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_APK_ASSET_PROVIDER_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_APK_ASSET_PROVIDER_H_

#include <android/asset_manager_jni.h>
#include <jni.h>

#include "appcode/assets/asset_resolver.h"
#include "appcode/fml/memory/ref_counted.h"
#include "appcode/fml/platform/android/scoped_java_ref.h"

namespace appcode {

class APKAssetProviderInternal {
 public:
  virtual std::unique_ptr<fml::Mapping> GetAsMapping(
      const std::string& asset_name) const = 0;

 protected:
  virtual ~APKAssetProviderInternal() = default;
};

class APKAssetProvider final : public AssetResolver {
 public:
  explicit APKAssetProvider(JNIEnv* env,
                            jobject assetManager,
                            std::string directory);

  explicit APKAssetProvider(std::shared_ptr<APKAssetProviderInternal> impl);

  ~APKAssetProvider() = default;

  // Returns a new 'std::unique_ptr<APKAssetProvider>' with the same 'impl_' as
  // this provider.
  std::unique_ptr<APKAssetProvider> Clone() const;

  // Obtain a raw pointer to the APKAssetProviderInternal.
  //
  // This method is intended for use in tests. Callers must not
  // delete the returned pointer.
  APKAssetProviderInternal* GetImpl() const { return impl_.get(); }

  bool operator==(const AssetResolver& other) const override;

 private:
  std::shared_ptr<APKAssetProviderInternal> impl_;

  // |appcode::AssetResolver|
  bool IsValid() const override;

  // |appcode::AssetResolver|
  bool IsValidAfterAssetManagerChange() const override;

  // |AssetResolver|
  AssetResolver::AssetResolverType GetType() const override;

  // |appcode::AssetResolver|
  std::unique_ptr<fml::Mapping> GetAsMapping(
      const std::string& asset_name) const override;

  // |AssetResolver|
  const APKAssetProvider* as_apk_asset_provider() const override {
    return this;
  }

  FML_DISALLOW_COPY_AND_ASSIGN(APKAssetProvider);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_APK_ASSET_PROVIDER_H_
