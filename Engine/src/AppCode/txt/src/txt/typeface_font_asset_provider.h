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

#ifndef APPCODE_TXT_SRC_TXT_TYPEFACE_FONT_ASSET_PROVIDER_H_
#define APPCODE_TXT_SRC_TXT_TYPEFACE_FONT_ASSET_PROVIDER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "appcode/fml/macros.h"
#include "third_party/skia/include/core/SkFontMgr.h"
#include "txt/font_asset_provider.h"

namespace txt {

class TypefaceFontStyleSet : public SkFontStyleSet {
 public:
  TypefaceFontStyleSet();

  ~TypefaceFontStyleSet() override;

  void registerTypeface(sk_sp<SkTypeface> typeface);

  // |SkFontStyleSet|
  int count() override;

  // |SkFontStyleSet|
  void getStyle(int index, SkFontStyle* style, SkString* name) override;

  // |SkFontStyleSet|
  sk_sp<SkTypeface> createTypeface(int index) override;

  // |SkFontStyleSet|
  sk_sp<SkTypeface> matchStyle(const SkFontStyle& pattern) override;

 private:
  std::vector<sk_sp<SkTypeface>> typefaces_;

  FML_DISALLOW_COPY_AND_ASSIGN(TypefaceFontStyleSet);
};

class TypefaceFontAssetProvider : public FontAssetProvider {
 public:
  TypefaceFontAssetProvider();
  ~TypefaceFontAssetProvider() override;

  void RegisterTypeface(sk_sp<SkTypeface> typeface);

  void RegisterTypeface(sk_sp<SkTypeface> typeface,
                        const std::string& family_name_alias);

  // |FontAssetProvider|
  size_t GetFamilyCount() const override;

  // |FontAssetProvider|
  std::string GetFamilyName(int index) const override;

  // |FontAssetProvider|
  sk_sp<SkFontStyleSet> MatchFamily(const std::string& family_name) override;

 private:
  std::unordered_map<std::string, sk_sp<TypefaceFontStyleSet>>
      registered_families_;
  std::vector<std::string> family_names_;

  FML_DISALLOW_COPY_AND_ASSIGN(TypefaceFontAssetProvider);
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_TYPEFACE_FONT_ASSET_PROVIDER_H_
