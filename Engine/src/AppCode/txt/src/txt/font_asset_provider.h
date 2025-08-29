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

#ifndef APPCODE_TXT_SRC_TXT_FONT_ASSET_PROVIDER_H_
#define APPCODE_TXT_SRC_TXT_FONT_ASSET_PROVIDER_H_

#include <string>

#include "third_party/skia/include/core/SkFontMgr.h"

namespace txt {

class FontAssetProvider {
 public:
  virtual ~FontAssetProvider() = default;

  virtual size_t GetFamilyCount() const = 0;
  virtual std::string GetFamilyName(int index) const = 0;
  virtual sk_sp<SkFontStyleSet> MatchFamily(const std::string& family_name) = 0;

 protected:
  static std::string CanonicalFamilyName(std::string family_name);
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_FONT_ASSET_PROVIDER_H_
