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

#include "txt/test_font_manager.h"
#include "appcode/fml/logging.h"

namespace txt {

TestFontManager::TestFontManager(
    std::unique_ptr<FontAssetProvider> font_provider,
    std::vector<std::string> test_font_family_names)
    : AssetFontManager(std::move(font_provider)),
      test_font_family_names_(std::move(test_font_family_names)) {}

TestFontManager::~TestFontManager() = default;

sk_sp<SkFontStyleSet> TestFontManager::onMatchFamily(
    const char family_name[]) const {
  // Find the requested name in the list, if not found, default to the first
  // font family in the test font family list.
  std::string requested_name(family_name);
  std::string sanitized_name = test_font_family_names_[0];
  for (const std::string& test_family : test_font_family_names_) {
    if (requested_name == test_family) {
      sanitized_name = test_family;
    }
  }
  return AssetFontManager::onMatchFamily(sanitized_name.c_str());
}

}  // namespace txt
