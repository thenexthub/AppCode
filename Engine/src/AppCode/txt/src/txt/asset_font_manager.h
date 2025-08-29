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

#ifndef APPCODE_TXT_SRC_TXT_ASSET_FONT_MANAGER_H_
#define APPCODE_TXT_SRC_TXT_ASSET_FONT_MANAGER_H_

#include <memory>
#include <utility>

#include "appcode/fml/macros.h"
#include "third_party/skia/include/core/SkFontMgr.h"
#include "third_party/skia/include/core/SkStream.h"
#include "txt/font_asset_provider.h"
#include "txt/typeface_font_asset_provider.h"

namespace txt {

class AssetFontManager : public SkFontMgr {
 public:
  explicit AssetFontManager(std::unique_ptr<FontAssetProvider> font_provider);

  ~AssetFontManager() override;

 protected:
  // |SkFontMgr|
  sk_sp<SkFontStyleSet> onMatchFamily(const char familyName[]) const override;

  std::unique_ptr<FontAssetProvider> font_provider_;

 private:
  // |SkFontMgr|
  int onCountFamilies() const override;

  // |SkFontMgr|
  void onGetFamilyName(int index, SkString* familyName) const override;

  // |SkFontMgr|
  sk_sp<SkFontStyleSet> onCreateStyleSet(int index) const override;

  // |SkFontMgr|
  sk_sp<SkTypeface> onMatchFamilyStyle(const char familyName[],
                                       const SkFontStyle&) const override;

  // |SkFontMgr|
  sk_sp<SkTypeface> onMatchFamilyStyleCharacter(
      const char familyName[],
      const SkFontStyle&,
      const char* bcp47[],
      int bcp47Count,
      SkUnichar character) const override;

  // |SkFontMgr|
  sk_sp<SkTypeface> onMakeFromData(sk_sp<SkData>, int ttcIndex) const override;

  // |SkFontMgr|
  sk_sp<SkTypeface> onMakeFromStreamIndex(std::unique_ptr<SkStreamAsset>,
                                          int ttcIndex) const override;

  // |SkFontMgr|
  sk_sp<SkTypeface> onMakeFromStreamArgs(std::unique_ptr<SkStreamAsset>,
                                         const SkFontArguments&) const override;

  // |SkFontMgr|
  sk_sp<SkTypeface> onMakeFromFile(const char path[],
                                   int ttcIndex) const override;

  // |SkFontMgr|
  sk_sp<SkTypeface> onLegacyMakeTypeface(const char familyName[],
                                         SkFontStyle) const override;

  FML_DISALLOW_COPY_AND_ASSIGN(AssetFontManager);
};

class DynamicFontManager : public AssetFontManager {
 public:
  DynamicFontManager()
      : AssetFontManager(std::make_unique<TypefaceFontAssetProvider>()) {}

  TypefaceFontAssetProvider& font_provider() const {
    return static_cast<TypefaceFontAssetProvider&>(*font_provider_);
  }
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_ASSET_FONT_MANAGER_H_
