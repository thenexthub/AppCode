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

#include "txt/asset_font_manager.h"

#include <memory>

#include "appcode/fml/logging.h"
#include "third_party/skia/include/core/SkString.h"
#include "third_party/skia/include/core/SkTypeface.h"

namespace txt {

AssetFontManager::AssetFontManager(
    std::unique_ptr<FontAssetProvider> font_provider)
    : font_provider_(std::move(font_provider)) {
  FML_DCHECK(font_provider_ != nullptr);
}

AssetFontManager::~AssetFontManager() = default;

int AssetFontManager::onCountFamilies() const {
  return font_provider_->GetFamilyCount();
}

void AssetFontManager::onGetFamilyName(int index, SkString* familyName) const {
  familyName->set(font_provider_->GetFamilyName(index).c_str());
}

sk_sp<SkFontStyleSet> AssetFontManager::onCreateStyleSet(int index) const {
  FML_DCHECK(false);
  return nullptr;
}

sk_sp<SkFontStyleSet> AssetFontManager::onMatchFamily(
    const char family_name_string[]) const {
  std::string family_name(family_name_string);
  return font_provider_->MatchFamily(family_name);
}

sk_sp<SkTypeface> AssetFontManager::onMatchFamilyStyle(
    const char familyName[],
    const SkFontStyle& style) const {
  sk_sp<SkFontStyleSet> font_style_set =
      font_provider_->MatchFamily(std::string(familyName));
  if (font_style_set == nullptr) {
    return nullptr;
  }
  return font_style_set->matchStyle(style);
}

sk_sp<SkTypeface> AssetFontManager::onMatchFamilyStyleCharacter(
    const char familyName[],
    const SkFontStyle&,
    const char* bcp47[],
    int bcp47Count,
    SkUnichar character) const {
  return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onMakeFromData(sk_sp<SkData>,
                                                   int ttcIndex) const {
  FML_DCHECK(false);
  return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onMakeFromStreamIndex(
    std::unique_ptr<SkStreamAsset>,
    int ttcIndex) const {
  FML_DCHECK(false);
  return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onMakeFromStreamArgs(
    std::unique_ptr<SkStreamAsset>,
    const SkFontArguments&) const {
  FML_DCHECK(false);
  return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onMakeFromFile(const char path[],
                                                   int ttcIndex) const {
  FML_DCHECK(false);
  return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onLegacyMakeTypeface(
    const char familyName[],
    SkFontStyle) const {
  return nullptr;
}

}  // namespace txt
