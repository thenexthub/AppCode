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

#ifndef APPCODE_LIB_UI_TEXT_ASSET_MANAGER_FONT_PROVIDER_H_
#define APPCODE_LIB_UI_TEXT_ASSET_MANAGER_FONT_PROVIDER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "appcode/assets/asset_manager.h"
#include "appcode/fml/macros.h"
#include "third_party/skia/include/core/SkFontMgr.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "txt/font_asset_provider.h"

namespace appcode {

class AssetManagerFontStyleSet : public SkFontStyleSet {
 public:
  AssetManagerFontStyleSet(std::shared_ptr<AssetManager> asset_manager,
                           std::string family_name);

  ~AssetManagerFontStyleSet() override;

  void registerAsset(const std::string& asset);

  // |SkFontStyleSet|
  int count() override;

  // |SkFontStyleSet|
  void getStyle(int index, SkFontStyle*, SkString* style) override;

  // |SkFontStyleSet|
  using CreateTypefaceRet =
      decltype(std::declval<SkFontStyleSet>().createTypeface(0));
  CreateTypefaceRet createTypeface(int index) override;

  // |SkFontStyleSet|
  using MatchStyleRet = decltype(std::declval<SkFontStyleSet>().matchStyle(
      std::declval<SkFontStyle>()));
  MatchStyleRet matchStyle(const SkFontStyle& pattern) override;

 private:
  std::shared_ptr<AssetManager> asset_manager_;
  std::string family_name_;

  struct TypefaceAsset {
    explicit TypefaceAsset(std::string a);

    TypefaceAsset(const TypefaceAsset& other);

    ~TypefaceAsset();

    std::string asset;
    sk_sp<SkTypeface> typeface;
  };
  std::vector<TypefaceAsset> assets_;

  FML_DISALLOW_COPY_AND_ASSIGN(AssetManagerFontStyleSet);
};

class AssetManagerFontProvider : public txt::FontAssetProvider {
 public:
  explicit AssetManagerFontProvider(
      std::shared_ptr<AssetManager> asset_manager);

  ~AssetManagerFontProvider() override;

  void RegisterAsset(const std::string& family_name, const std::string& asset);

  // |FontAssetProvider|
  size_t GetFamilyCount() const override;

  // |FontAssetProvider|
  std::string GetFamilyName(int index) const override;

  // |FontAssetProvider|
  sk_sp<SkFontStyleSet> MatchFamily(const std::string& family_name) override;

 private:
  std::shared_ptr<AssetManager> asset_manager_;
  std::unordered_map<std::string, sk_sp<AssetManagerFontStyleSet>>
      registered_families_;
  std::vector<std::string> family_names_;

  FML_DISALLOW_COPY_AND_ASSIGN(AssetManagerFontProvider);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_TEXT_ASSET_MANAGER_FONT_PROVIDER_H_
