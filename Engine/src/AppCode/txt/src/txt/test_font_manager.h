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

#ifndef APPCODE_TXT_SRC_TXT_TEST_FONT_MANAGER_H_
#define APPCODE_TXT_SRC_TXT_TEST_FONT_MANAGER_H_

#include <memory>
#include <string>
#include <vector>

#include "appcode/fml/macros.h"
#include "third_party/skia/include/core/SkFontMgr.h"
#include "txt/asset_font_manager.h"
#include "txt/font_asset_provider.h"

namespace txt {

// A font manager intended for tests that matches all requested fonts using
// one family.
class TestFontManager : public AssetFontManager {
 public:
  TestFontManager(std::unique_ptr<FontAssetProvider> font_provider,
                  std::vector<std::string> test_font_family_names);

  ~TestFontManager() override;

 private:
  std::vector<std::string> test_font_family_names_;

  sk_sp<SkFontStyleSet> onMatchFamily(const char family_name[]) const override;

  FML_DISALLOW_COPY_AND_ASSIGN(TestFontManager);
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_TEST_FONT_MANAGER_H_
